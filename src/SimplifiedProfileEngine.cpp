#include "../include/SimplifiedProfileEngine.h"

#include "../include/ExitTrigger.h"

bool heating_finished()
{
    return true;
}

bool has_reached_final_weight() {
    return false;
}

void setTargetWeight(double setPoint)
{
    printf("Setting target weight to %f\n", setPoint);
}

void setTargetTemperature(double setPoint)
{
    printf("Setting target temperature to %f\n", setPoint);
}

void setTargetPressure(double setPoint)
{
    printf("Setting target pressure to %f\n", setPoint);
}

void setLimitedPressure(double setPoint)
{
    printf("Setting target pressure limit to %f\n", setPoint);
}

void setTargetFlow(double setPoint)
{
    printf("Setting target flow to %f\n", setPoint);
}

void setLimitedFlow(double setPoint)
{
    printf("Setting flow limit to %f\n", setPoint);
}

void setTargetPower(double setPoint)
{
    printf("Setting target power to %f\n", setPoint);
}

void setTargetPistonPosition(double setPoint)
{
    printf("Setting target piston position to %f\n", setPoint);
}

enum
{
    STAGE_ENTRY,
    STAGE_EXIT,
};

void SimplifiedProfileEngine::saveStageLog(bool is_stage_exit, long timestamp)
{
    printf("Saving %s log for stage %ld. Timestamp = %ld\n", is_stage_exit == STAGE_ENTRY ? "START" : "EXIT", this->currentStageId, timestamp);
    StageLog *log = &this->profile->stage_log[this->currentStageId];
    StageVariables *vars = is_stage_exit == STAGE_ENTRY ? &log->start : &log->end;

    vars->flow = writeProfileFlow(this->driver->get_sensor_data().water_flow);
    vars->piston_position = writeProfilePercent(this->driver->get_sensor_data().piston_position);
    vars->pressure = writeProfilePressure(this->driver->get_sensor_data().water_pressure);
    vars->timestamp = timestamp;

    log->valid = true;
}

void SimplifiedProfileEngine::step()
{
    if (this->profile->stages_len == 0)
    {
        this->state = ProfileState::ERROR;
        throw new NoStagesInProfileException();
    }

    switch (this->state)
    {
    case ProfileState::IDLE:
        break;
    case ProfileState::START:
        this->state = ProfileState::HEATING;
        break;

    case ProfileState::HEATING:
        setTargetTemperature(
            parseProfileTemperature(this->profile->temperature));
        setTargetWeight(parseProfileWeight(this->profile->finalWeight));
        if (heating_finished())
        {
            this->state = ProfileState::READY;
        }
        break;
    case ProfileState::READY:
        this->currentStageId = 0;
        if (this->profile->wait_after_heating)
        {
        }
        else
        {
            this->state = ProfileState::RETRACTING;
        }
        break;
    case ProfileState::RETRACTING:
        setTargetPistonPosition(0);
        if (this->driver->get_sensor_data().piston_position <= 1)
        {
            this->state = ProfileState::BREWING;
            this->profileStartTimestamp = std::chrono::high_resolution_clock::now();
            saveStageLog(STAGE_ENTRY, 0);
        }
        break;
    case ProfileState::BREWING:
        this->state = this->processStageStep();
        break;
    case ProfileState::DONE:
        if (this->profile->auto_purge)
        {
            this->state = ProfileState::RETRACTING;
        }
        else
        {
        }
        break;
    case ProfileState::PURGING:
        setTargetPistonPosition(100);
        if (driver->get_sensor_data().piston_position >= 99)
        {
            this->state = ProfileState::END;
        }
        break;
    case ProfileState::END:
        // This is where we do enable the loopy-de-loop functionality
        this->state = ProfileState::IDLE;
        break;
    case ProfileState::ERROR:
        break;
    }
}

ProfileState SimplifiedProfileEngine::transitionStage(size_t target_stage)
{
    auto end_time = std::chrono::high_resolution_clock::now();
    auto time_passed_ms = (end_time - this->profileStartTimestamp) / std::chrono::milliseconds(1);

    saveStageLog(STAGE_EXIT, time_passed_ms);

    if (target_stage == this->currentStageId)
    {
        printf("Profile End reached via stage end\n");
        return ProfileState::DONE;
    }

    this->currentStageId = target_stage;
    if (this->currentStageId >= this->profile->stages_len)
    {
        printf("Next StageID unreachable");
        return ProfileState::DONE;
    }
    saveStageLog(STAGE_ENTRY, time_passed_ms);

    return ProfileState::BREWING;
}

ProfileState SimplifiedProfileEngine::processStageStep()
{
    if (this->currentStageId >= this->profile->stages_len)
    {
        printf("StageID unreachable");
        return ProfileState::ERROR;
    }

    if (has_reached_final_weight())
    {
        printf("Profile End reached via final weight hit\n");
        return ProfileState::DONE;
    }

    printf("executing stage=%d\n", (short)this->currentStageId);

    auto now = std::chrono::high_resolution_clock::now();
    auto profile_time_passed = (now - this->profileStartTimestamp) / std::chrono::milliseconds(1);

    const Stage *stage = &this->profile->stages[this->currentStageId];
    const StageLog *log = &this->profile->stage_log[this->currentStageId];
    if (!log->valid)
    {
        saveStageLog(STAGE_ENTRY, profile_time_passed);
    }

    // Ensure the sampler is fed with the right stage
    if (this->sampler.stageId != this->currentStageId)
    {
        this->sampler.load_new_stage(stage, this->currentStageId);
    }

    auto stage_timestamp = (now -
            (this->profileStartTimestamp +
                (log->start.timestamp * std::chrono::milliseconds(1))
            )
         ) / std::chrono::milliseconds(1);

    for (size_t i = 0; i < stage->exitTrigger_len; i++)
    {
        const ExitTrigger *trigger = &stage->exitTrigger[i];
        bool should_exit = checkExitCondition(trigger, this->driver, stage_timestamp, profile_time_passed);
        if (should_exit)
        {
            printf("Exit trigger activated!\n");
            printf("________________________________________________________________\n");
            printf("current stage_id: %zu, target_stage: %d\n", this->currentStageId, trigger->target_stage);
            printf("________________________________________________________________\n");

            return this->transitionStage(trigger->target_stage);
        }
    }

    long input_reference_value = 0;
    switch (stage->dynamics.inputSelect)
    {
    case InputType::INPUT_TIME:
        input_reference_value = stage_timestamp;
        break;
    case InputType::INPUT_PISTON_POSITION:
        input_reference_value = this->driver->get_sensor_data().piston_position;
        break;
    case InputType::INPUT_WEIGHT:
        input_reference_value = this->driver->get_sensor_data().weight;
        break;
    default:
        printf("Invalid input reference type! Aborting");
        return ProfileState::ERROR;
    }

    double sampled_output = sampler.get(input_reference_value);
    printf("sampled (%ld,%f)\n", input_reference_value, sampled_output);
    printf("Setting output at %ld ms to %f\n", profile_time_passed, sampled_output);

    // Dont use the parsed value for limiter checks here as the
    // float might not be perfectly encoding zero
    if (stage->dynamics.limits.flow > 0)
    {
        auto flow_limit = parseProfileFlow(stage->dynamics.limits.flow);
        setLimitedFlow(flow_limit);
    }
    if (stage->dynamics.limits.pressure > 0)
    {
        auto pressure_limit = parseProfilePressure(stage->dynamics.limits.pressure);
        setLimitedFlow(pressure_limit);
    }

    switch (stage->dynamics.controlSelect)
    {
    case ControlType::CONTROL_PRESSURE:
        setTargetPressure(sampled_output);
        break;
    case ControlType::CONTROL_FLOW:
        setTargetFlow(sampled_output);
        break;
    case ControlType::CONTROL_POWER:
        setTargetPower(sampled_output);
        break;
    case ControlType::CONTROL_PISTON_POSITION:
        setTargetPistonPosition(sampled_output);
        break;
    default:
        printf("Invalid control method! Aborting");
        return ProfileState::ERROR;
    }

    return ProfileState::BREWING;
}

SimplifiedProfileEngine :: ~SimplifiedProfileEngine() {
    Profile *prof = this->profile;
    freeProfile(prof);
}