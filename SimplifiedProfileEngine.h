#ifndef __SIMPLIFIED_PROFILE_ENGINE_H__
#define __SIMPLIFIED_PROFILE_ENGINE_H__

#include "Sensor.h"
#include "Sampler.h"
#include "ProfileDefinition.h"
#include <exception>
#include <chrono>

enum class ProfileState
{
    IDLE,

    START,
    HEATING,
    READY,
    RETRACTING,
    BREWING,
    DONE,
    PURGING,
    END,

    ERROR,
};

struct NoStagesInProfileException : std::exception {};

class SimplifiedProfileEngine
{
    Profile *profile;
    Driver *driver;
    ProfileState processStageStep();

    size_t currentStageId = 0;
    Sampler sampler;
    void saveStageLog(bool is_stage_entry, long timestamp);
    ProfileState transitionStage(size_t target_stage);
    std::chrono::high_resolution_clock::time_point profileStartTimestamp;

public:
    SimplifiedProfileEngine(Profile *ext_profile, Driver *ext_driver) :  profile(ext_profile), driver(ext_driver), state(ProfileState::IDLE) {}
    ~SimplifiedProfileEngine();

    void start() {
        this->currentStageId = 0;
        this->state = ProfileState::HEATING;
    }
    void step();

    ProfileState state;
};

#endif