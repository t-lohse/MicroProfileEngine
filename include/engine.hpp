//
// Created by lohse on 11/28/24.
//

#ifndef MICROPROFILEENGINE_ENGINE_HPP
#define MICROPROFILEENGINE_ENGINE_HPP
#include "sensor.hpp"
#include "Profile.hpp"

#include <expected>

enum class ProfileState {
    Start,
    Heating,
    Ready,
    Retracting,
    Brewing,
    Done,
    Purging,
};

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
    ProfileEngineRunning(Driver<T> driver, gsl::not_null<profile::Profile*> profile);

    void saveStageLog(std::optional<std::chrono::time_point<std::chrono::system_clock>> exitTime);
    ProfileState transitionState(uint8_t targetStage);
    std::expected<ProfileState, std::string> processStageStep();

    friend class ProfileEngineIdle<T>;
    friend class EngineStepResult<T>;

public:
    ProfileEngineRunning() = delete;
    ProfileEngineRunning(ProfileEngineRunning&) = delete;
    ProfileEngineRunning& operator=(ProfileEngineRunning&) = delete;
    ProfileEngineRunning(ProfileEngineRunning&&) = delete;
    ProfileEngineRunning& operator=(ProfileEngineRunning&&) = delete;
    ~ProfileEngineRunning() = default;

    EngineStepResult<T> step() &&;
    ProfileState getState() const;
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
    ProfileEngineIdle(Driver<T>&& driver, gsl::not_null<profile::Profile*> profile);

    ProfileEngineRunning<T> start() &&;
};

template <SensorState T>
class EngineStepResult
{
    std::variant<ProfileEngineRunning<T>, ProfileEngineIdle<T>, std::string> value;

    friend class ProfileEngineRunning<T>;
    friend class ProfileEngineIdle<T>;
    // EngineStepResult(ProfileEngineRunning<T>);
    EngineStepResult(ProfileEngineRunning<T>&&);
    // EngineStepResult(ProfileEngineIdle<T>);
    EngineStepResult(ProfileEngineIdle<T>&&);
    // EngineStepResult(std::string);
    EngineStepResult(std::string&&);

public:
    EngineStepResult() = delete;
    EngineStepResult(EngineStepResult&) = delete;
    EngineStepResult& operator=(EngineStepResult&) = delete;
    EngineStepResult(EngineStepResult&&) = delete;
    EngineStepResult& operator=(EngineStepResult&&) = delete;
    ~EngineStepResult() = default;
};

#endif  // MICROPROFILEENGINE_ENGINE_HPP
