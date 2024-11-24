
#include "../include/ExitTrigger.h"

static double getExitInput(const ExitTrigger *exit, Driver *driver, long stage_timestamp, long profile_timestamp)
{

    switch (exit->type)
    {
    case ExitType::EXIT_PRESSURE:
        return driver->get_sensor_data().water_pressure;

    case ExitType::EXIT_FLOW:
        return driver->get_sensor_data().water_flow;

    case ExitType::EXIT_TEMPERATURE:
        return driver->get_sensor_data().stable_temperature;

    case ExitType::EXIT_WEIGHT:
        return driver->get_sensor_data().weight;

    case ExitType::EXIT_PISTON_POSITION:
        return driver->get_sensor_data().piston_position;

    case ExitType::EXIT_BUTTON:
        return driver->get_button_gesture("Encoder Button", "Single Tap");

    case ExitType::EXIT_POWER:
        throw new std::runtime_error("EXIT_POWER unimplemented");

    case ExitType::EXIT_TIME:
        return (
                   exit->reference == ExitReferenceType::EXIT_REF_ABSOLUTE ? profile_timestamp : stage_timestamp) /
               1000.0;

    default:
        throw new UnsupportedExitCondition("Unnown Condition");
    }
}

bool checkExitCondition(const ExitTrigger *exit, Driver *driver, long stage_timestamp, long profile_timestamp)
{
    double current_value = getExitInput(exit, driver, stage_timestamp, profile_timestamp);
    double exit_value = parseExitValue(exit->value);

    printf("Input: %f -- Exit Value: %f\n", current_value, exit_value);


    // printf("ExitTrigger: Comparing %f and %f == %d\n", current_value, exit_value, current_value <= exit_value);
    switch (exit->comparison)
    {
    case ExitComparison::EXIT_COMP_SMALLER:
        if (current_value <= exit_value)
        {
            printf("ExitTrigger Type=%d: %f <= %f\n", static_cast<int>(exit->type), current_value, exit_value);
            return true;
        }
        break;
    case ExitComparison::EXIT_COMP_GREATER:
        if (current_value >= exit_value)
        {
            printf("ExitTrigger Type=%d: %f >= %f\n", static_cast<int>(exit->type), current_value, exit_value);
            return true;
        }
        break;
    default:
        throw new UnsupportedExitCondition("Unnown Comparison");
    }
    return false;
}
