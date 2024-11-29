//
// Created by lohse on 11/28/24.
//

#ifndef MICROPROFILEENGINE_ENGINE_HPP
#define MICROPROFILEENGINE_ENGINE_HPP
#include <math.h>

#include <expected>

#include "sensor.hpp"
#include "Profile.hpp"
#include "Stage.hpp"

enum class ProfileState {
    Start,
    Heating,
    Ready,
    Retracting,
    Brewing,
    Done,
    Purging,
};
std::ostream& operator<<(std::ostream& os, const ProfileState& obj)
{
    switch (obj) {
    case ProfileState::Start: os << "Start"; break;
    case ProfileState::Heating: os << "Heating"; break;
    case ProfileState::Ready: os << "Ready"; break;
    case ProfileState::Retracting: os << "Retracting"; break;
    case ProfileState::Brewing: os << "Brewing"; break;
    case ProfileState::Done: os << "Done"; break;
    case ProfileState::Purging: os << "Purging"; break;
    }
    return os;
}

template <SensorState T>
class ProfileEngineIdle;
template <SensorState T>
class EngineStepResult;

template <SensorState T>
class ProfileEngineRunning
{
    Driver<T> driver;
    gsl::not_null<profile::Profile*> profile;

    std::chrono::time_point<std::chrono::system_clock> profileStartTime, stageStartTime;
    ProfileState state;
    uint8_t currentStageId;
    ProfileEngineRunning(Driver<T>&& driver, gsl::not_null<profile::Profile*> profile):
        driver{std::move(driver)}, profile(profile), profileStartTime{std::chrono::system_clock::now()},
        stageStartTime(std::chrono::system_clock::now()), state{ProfileState::Heating}, currentStageId{1}
    {}

    void saveStageLog(std::optional<std::chrono::time_point<std::chrono::system_clock>> exitTime)
    {
        auto& log = profile->getStageLogs()[currentStageId];

        auto time = exitTime ? *exitTime : std::chrono::system_clock::now();
        auto vars =
            profile::StageVariables(driver.getSensorState().waterFlow(), driver.getSensorState().pistonPosition(),
                                    driver.getSensorState().waterPressure(), time);
        if (exitTime) {
            std::cout << "Saving EXIT log for stage " << static_cast<int>(currentStageId) << ". Timestamp = " << time
                      << std::endl;
            log.putExit(vars);
        } else {
            std::cout << "Saving ENTRY log for stage " << static_cast<int>(currentStageId) << ". Timestamp = " << time
                      << std::endl;
            log.putEntry(vars);
        }
    }

    ProfileState transitionState(uint8_t targetStage)
    {
        using PS = ProfileState;
        saveStageLog(stageStartTime);
        currentStageId = targetStage;

        if (profile->getStages().contains(currentStageId)) {
            stageStartTime = std::chrono::system_clock::now();
            return PS::Brewing;
        } else {
            std::cout << "Profile End reached via stage end" << std::endl;
            return PS::Done;
        }
    }
    std::expected<ProfileState, std::string> processStageStep()
    {
        using PS = ProfileState;
        if (driver.hasReachedFinalWeight()) {
            std::cout << "Profile End reached via final weight hit" << std::endl;
            return PS::Done;
        }
        std::cout << "executing stage=" << static_cast<int>(currentStageId) << std::endl;

        std::chrono::duration<double> elapsed = std::chrono::system_clock::now() - profileStartTime;
        if (profile->getStageLogs()[currentStageId].isValid())
            saveStageLog({});

        const auto& stage = profile->getStages().at(currentStageId);
        const auto& exitTriggers = stage.getExitTriggers();

        for (const auto& trigger : exitTriggers) {
            if (trigger.checkCond(driver, stageStartTime, profileStartTime)) {
                std::cout << "EXIT COND" << std::endl;

                auto target = trigger.targetStage();

                return transitionState(target.value_or(currentStageId + 1));
                // return target ? transitionState(*target) : transitionState(currentStageId + 1);
            }
        }

        auto stageDyn = gsl::not_null(&stage.getDynamics());

        double inputRefVal = NAN;  // = std::numeric_limits<double>::min();
        switch (stageDyn->inputType()) {
            using IT = profile::InputType;
        case IT::Time: inputRefVal = elapsed.count(); break;
        case IT::PistonPosition: inputRefVal = driver.getSensorState().pistonPosition(); break;
        case IT::Weight: inputRefVal = driver.getSensorState().weight(); break;
        }

        auto sampledOutput = stageDyn->runInterpolation(inputRefVal);
        const double SEC_TO_MILLIS = 1000.0;
        std::cout << "sampled (" << inputRefVal << ") (" << sampledOutput << ")" << std::endl;
        std::cout << "Setting output at " << elapsed.count() * SEC_TO_MILLIS << " ms to " << sampledOutput << std::endl;

        for (const auto& l : stage.getLimits()) {
            auto v = *l;
            switch (l) {
                using LT = profile::Limit;
            case LT::Pressure: driver.setPressureLimit(v); break;
            case LT::Flow: driver.setFlowLimit(v); break;
            }
        }

        switch (stage.getControlType()) {
            using CT = profile::ControlType;
        case CT::Pressure: driver.setTargetPressure(sampledOutput); break;
        case CT::Flow: driver.setTargetFlow(sampledOutput); break;
        case CT::Power: driver.setTargetPower(sampledOutput); break;
        case CT::PistonPosition: driver.setTargetPistonPosition(sampledOutput); break;
        }

        return PS::Brewing;
    }

    friend class ProfileEngineIdle<T>;
    friend class EngineStepResult<T>;

public:
    ProfileEngineRunning() = delete;
    ProfileEngineRunning(ProfileEngineRunning&) = delete;
    ProfileEngineRunning& operator=(ProfileEngineRunning&) = delete;
    ProfileEngineRunning(ProfileEngineRunning&&) = default;
    ProfileEngineRunning& operator=(ProfileEngineRunning&&) = default;
    ~ProfileEngineRunning() = default;

    EngineStepResult<T> step() &&
    {
        using PS = ProfileState;
        const double PISTON_LOWER_BOUND = 1.0;
        const double PISTON_UPPER_BOUND = 99.0;
        switch (state) {
        case PS::Start: state = PS::Heating; break;
        case PS::Heating:
            driver.setTargetTemperature(profile->getStartingTemp());
            driver.setTargetWeight(profile->getTargetWeight());
            if (driver.heatingFinished())
                state = PS::Ready;
            break;
        case PS::Ready:
            currentStageId = 1;
            if (!profile->shouldWaitAfterHeating())
                state = PS::Retracting;
            break;
        case PS::Retracting:
            driver.setTargetPistonPosition(0.0);
            if (driver.getSensorState().pistonPosition() <= PISTON_LOWER_BOUND) {
                state = PS::Brewing;
                profileStartTime = std::chrono::system_clock::now();
                stageStartTime = std::chrono::system_clock::now();
            }
            break;
        case PS::Brewing: {
            auto nextState = processStageStep();
            if (nextState) {
                state = *nextState;
            } else {
                return EngineStepResult<T>(std::move(nextState).error());
            }
            break;
        }
        case PS::Done:
            if (profile->shouldAutoPurge())
                state = PS::Retracting;
            break;
        case PS::Purging:
            driver.setTargetPistonPosition(100.0);
            if (driver.getSensorState().pistonPosition() >= PISTON_UPPER_BOUND) {
                return EngineStepResult<T>(ProfileEngineIdle<T>(std::move(driver), profile));
            }
            break;
        }

        return EngineStepResult<T>(std::move(*this));
    }
    ProfileState getState() const { return state; }
};

template <SensorState T>
class ProfileEngineIdle
{
    Driver<T> driver;
    gsl::not_null<profile::Profile*> profile;

    friend class ProfileEngineRunning<T>;
    friend class EngineStepResult<T>;

public:
    ProfileEngineIdle() = delete;
    ProfileEngineIdle(ProfileEngineIdle&) = delete;
    ProfileEngineIdle& operator=(ProfileEngineIdle&) = delete;
    ProfileEngineIdle(ProfileEngineIdle&&) = default;
    ProfileEngineIdle& operator=(ProfileEngineIdle&&) = default;
    ~ProfileEngineIdle() = default;
    ProfileEngineIdle(Driver<T>&& driver, gsl::not_null<profile::Profile*> profile):
        driver{std::move(driver)}, profile(profile)
    {}

    ProfileEngineRunning<T> start() && { return ProfileEngineRunning<T>(std::move(driver), profile); }
};

template <SensorState T>
class EngineStepResult
{
    std::variant<ProfileEngineRunning<T>, ProfileEngineIdle<T>, std::string> value;

    friend class ProfileEngineRunning<T>;
    friend class ProfileEngineIdle<T>;
    // EngineStepResult(ProfileEngineRunning<T>) {}
    EngineStepResult(ProfileEngineRunning<T>&& v): value(std::move(v)) {}
    // EngineStepResult(ProfileEngineIdle<T>) {}
    EngineStepResult(ProfileEngineIdle<T>&& v): value(std::move(v)) {}
    // EEngineStepResult(std::string) {}
    EngineStepResult(std::string&& v): value(std::move(v)) {}

public:
    EngineStepResult() = delete;
    EngineStepResult(EngineStepResult&) = delete;
    EngineStepResult& operator=(EngineStepResult&) = delete;
    EngineStepResult(EngineStepResult&&) = delete;
    EngineStepResult& operator=(EngineStepResult&&) = delete;
    ~EngineStepResult() = default;

    enum class Type : uint8_t { Next = 0, Finished = 1, Error = 2 };

    using Type::Next;
    using Type::Finished;
    using Type::Error;

    operator Type() const { return static_cast<Type>(value.index()); }

    ProfileEngineRunning<T> getNext() && { return std::get<ProfileEngineRunning<T>>(std::move(value)); }

    [[maybe_unused]] ProfileEngineIdle<T> getFinished() &&
    {
        return std::get<ProfileEngineRunning<T>>(std::move(value));
    }
    std::string getError() && { return std::get<std::string>(std::move(value)); }
};

#endif  // MICROPROFILEENGINE_ENGINE_HPP
