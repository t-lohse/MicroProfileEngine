#include "ProfileDefinition.h"

#include <cstdlib>

double parseProfileFlow(flow_t flow)
{
    return flow / 10.0;
}

double parseProfilePressure(pressure_t pressure)
{
    return pressure / 10.0;
}

double parseProfilePercent(percent_t percent)
{
    return percent;
}

double parseProfileTemperature(temperature_t temperature)
{
    return temperature / 10.0;
}

double parseProfileWeight(weight_t weight)
{
    return weight / 10.0;
}

double parseProfileTime(timestamp_t time)
{
    return time / 10.0;
}

double parseExitValue(uint32_t val)
{
    return val;
}

flow_t writeProfileFlow(double flow)
{
    return static_cast<flow_t>(flow * 10);
}
pressure_t writeProfilePressure(double pressure)
{
    return static_cast<pressure_t>(pressure * 10);
}
percent_t writeProfilePercent(double percent)
{
    return static_cast<percent_t>(percent * 10);
}
temperature_t writeProfileTemperature(double temperature)
{
    return static_cast<temperature_t>(temperature * 10);
}
weight_t writeProfileWeight(double weight)
{
    return static_cast<weight_t>(weight * 10);
}
timestamp_t writeProfileTime(double time)
{
    return static_cast<timestamp_t>(time * 10);
}
uint32_t writeExitValue(double exit)
{
    return static_cast<uint32_t>(exit);
}

void freeProfile(Profile* profile) {
    if (profile->stages) {
        for (int stage_index = 0; stage_index < profile->stages_len; stage_index++) {
            const Stage *stage = &profile->stages[stage_index];
            free(stage->exitTrigger);
            free(stage->dynamics.points);
        }
        free(profile->stages);
    }
    if (profile->stage_log)
        free(profile->stage_log);
}