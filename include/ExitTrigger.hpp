#ifndef __PROFILE_EXIT_TRIGGER__
#define __PROFILE_EXIT_TRIGGER__

#include <iostream>
#include <map>
#include <vector>

#include "ProfileDefinition.hpp"
#include "Sensor.hpp"

struct UnsupportedExitCondition : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

bool checkExitCondition(const ExitTrigger* exit, Driver* driver, long stage_timestamp, long profile_timestamp);

#endif