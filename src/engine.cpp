//
// Created by lohse on 11/28/24.
//

#include "engine.hpp"
#include "Stage.hpp"

#include "iostream"
template <SensorState T>
ProfileEngineRunning<T>::ProfileEngineRunning(Driver<T> driver, gsl::not_null<profile::Profile*> profile):
    driver{driver}, profile(profile), profileStartTime{std::chrono::system_clock::now()},
    stageStartTime(std::chrono::system_clock::now()), state{ProfileState::Heating}, currentStageId{1}
{}

template <SensorState T>
EngineStepResult<T> ProfileEngineRunning<T>::step() &&
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
            return EngineStepResult(nextState.error());
        }
        break;
    }
    case PS::Done:
        if (profile->shouldAutoPurge())
            state = PS::Retracting;
        break;
    case PS::Purging:
        driver.setTargetPistonPosition(100.0);
        if (driver.getSensorState().piston_position() >= PISTON_UPPER_BOUND) {
            return EngineStepResult<T>(ProfileEngineIdle<T>(driver, profile));
        }
        break;
    }

    return EngineStepResult(this);
}
template <SensorState T>
ProfileState ProfileEngineRunning<T>::getState() const
{
    return state;
}
template <SensorState T>
std::expected<ProfileState, std::string> ProfileEngineRunning<T>::processStageStep()
{
    using PS = ProfileState;
    if (driver.hasReachedFinalWeight()) {
        std::cout << "Profile End reached via final weight hit" << std::endl;
        return PS::Done;
    }
    std::cout << "executing stage=" << currentStageId << std::endl;

    std::chrono::duration<double> elapsed = profileStartTime - std::chrono::system_clock::now();
    if (profile->getStageLogs()[currentStageId].isValid())
        saveStageLog({});

    const auto& stage = profile->getStages().at(currentStageId);
    const auto& exitTriggers = stage.getExitTriggers();

    for (const auto& trigger : exitTriggers) {
        if (trigger.checkCond(driver, stageStartTime, profileStartTime)) {
            std::cout << "EXIT COND" << std::endl;

            auto target = trigger.targetStage();
            return target ? transitionState(*target) : transitionState(currentStageId + 1);
        }
    }

    auto stageDyn = gsl::not_null(&stage.getDynamics());

    double inputRefVal = NAN;
    switch (stageDyn->inputType()) {
        using IT = profile::InputType;
    case IT::TIME: inputRefVal = elapsed.count(); break;
    case IT::PISTON_POSITION: inputRefVal = driver.getSensorState().pistonPosition(); break;
    case IT::WEIGHT: inputRefVal = driver.getSensorState().weight(); break;
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

template <SensorState T>
ProfileState ProfileEngineRunning<T>::transitionState(uint8_t targetStage)
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

template <SensorState T>
void ProfileEngineRunning<T>::saveStageLog(std::optional<std::chrono::time_point<std::chrono::system_clock>> exitTime)
{
    auto& log = profile->getStageLogs()[currentStageId];

    auto vars = StageVariables(driver.getSensorState().waterFlow(), driver.getSensorState().pistonPosition(),
                               driver.getSensorState().waterPressure(),
                               exitTime ? *exitTime : std::chrono::system_clock::now());
    if (exitTime) {
        std::cout << "Saving EXIT log for stage " << currentStageId << ". Timestamp = {:?}" << time << std::endl;
        log.putExit(vars);
    } else {
        std::cout << "Saving ENTRY log for stage " << currentStageId << ". Timestamp = {:?}" << time << std::endl;
        log.putEntry(vars);
    }
}

// ProfileEngineIdle
template <SensorState T>
ProfileEngineRunning<T> ProfileEngineIdle<T>::start() &&
{
    return ProfileEngineRunning(driver, profile);
}

template <SensorState T>
ProfileEngineIdle<T>::ProfileEngineIdle(Driver<T> driver, gsl::not_null<profile::Profile*> profile):
    driver{driver}, profile(profile)
{}

// template<SensorState T>
// EngineStepResult<T>::EngineStepResult(ProfileEngineRunning<T>) {}
template <SensorState T>
EngineStepResult<T>::EngineStepResult(ProfileEngineRunning<T>&& v): value(std::move(v))
{}
// template<SensorState T>
// EngineStepResult<T>::EngineStepResult(ProfileEngineIdle<T>) {}
template <SensorState T>
EngineStepResult<T>::EngineStepResult(ProfileEngineIdle<T>&& v): value(std::move(v))
{}
// template<SensorState T>
// EngineStepResult<T>::EngineStepResult(std::string) {}
template <SensorState T>
EngineStepResult<T>::EngineStepResult(std::string&& v): value(std::move(v))
{}