#ifndef __PROFILE_TYPES_H__
#define __PROFILE_TYPES_H__

#include <cstdint>
#include <cstdio>

#define ENTRY_MAX_BITS(PREFIX, MAX_VALUE, BITS)                                                                      \
    constexpr int PREFIX##_MAX_BITS = BITS;                                                                          \
    static_assert((1 << BITS) > (static_cast<int>(MAX_VALUE) - 1), #PREFIX "_MAX_BITS is not leaving enough space"); \
    static_assert((1 << (BITS - 1)) <= static_cast<int>(MAX_VALUE) + 1, #PREFIX "_MAX_BITS  is wasting space");

#define ENUM_MAX_BITS(ENUM_TYPE, MAX_ENUM_ENTRY, BITS) \
    ENTRY_MAX_BITS(ENUM_TYPE, ENUM_TYPE::MAX_ENUM_ENTRY, BITS)

#define MAX_STAGES 128
ENTRY_MAX_BITS(STAGES, MAX_STAGES, 7)

#define MAX_PROFILE_TIME 24 // hours
#define MAX_PROFILE_TIME_VALUE (MAX_PROFILE_TIME * 60 * 60 * 10)
ENTRY_MAX_BITS(PROFILE_REFERENCE, MAX_PROFILE_TIME_VALUE, 20)

typedef uint8_t flow_t;
typedef uint8_t pressure_t;
typedef uint8_t percent_t;
typedef uint16_t temperature_t;
typedef uint16_t weight_t;
typedef uint16_t timestamp_t;

double parseProfileFlow(flow_t flow);
double parseProfilePressure(pressure_t pressure);
double parseProfilePercent(percent_t percent);
double parseProfileTemperature(temperature_t temperature);
double parseProfileWeight(weight_t weight);
double parseProfileTime(timestamp_t time);
double parseExitValue(uint32_t val);

flow_t writeProfileFlow(double flow);
pressure_t writeProfilePressure(double pressure);
percent_t writeProfilePercent(double percent);
temperature_t writeProfileTemperature(double temperature);
weight_t writeProfileWeight(double weight);
timestamp_t writeProfileTime(double time);
uint32_t writeExitValue(double exit);

enum class ControlType : uint8_t
{
    CONTROL_PRESSURE,
    CONTROL_FLOW,
    CONTROL_POWER,
    CONTROL_PISTON_POSITION,
    // FIXME MESSAGES?
};
ENUM_MAX_BITS(ControlType, CONTROL_PISTON_POSITION, 2)

enum class InputType : uint8_t
{
    INPUT_TIME,
    INPUT_PISTON_POSITION,
    INPUT_WEIGHT
};
ENUM_MAX_BITS(InputType, INPUT_WEIGHT, 2)

enum class InterpolationType : uint8_t
{
    INTERPOLATION_LINEAR,
    INTERPOLATION_CATMULL,
    INTERPOLATION_BEZIER
};
ENUM_MAX_BITS(InterpolationType, INTERPOLATION_BEZIER, 2)

enum class ExitType : uint8_t
{
    EXIT_PRESSURE,
    EXIT_FLOW,
    EXIT_TIME,
    EXIT_WEIGHT,
    EXIT_PISTON_POSITION,
    EXIT_POWER,
    EXIT_TEMPERATURE,
    EXIT_BUTTON,
};
ENUM_MAX_BITS(ExitType, EXIT_TEMPERATURE, 3);

enum class ExitComparison : uint8_t
{
    EXIT_COMP_SMALLER,
    EXIT_COMP_GREATER,
};
ENUM_MAX_BITS(ExitComparison, EXIT_COMP_GREATER, 1)

enum class ExitReferenceType : uint8_t
{
    EXIT_REF_ABSOLUTE,
    EXIT_REF_SELF,
};
ENUM_MAX_BITS(ExitReferenceType, EXIT_REF_SELF, 1)

struct StageVariables
{
    flow_t flow;
    pressure_t pressure;
    percent_t piston_position;
    uint32_t timestamp : PROFILE_REFERENCE_MAX_BITS;
} __attribute__((__packed__));

struct StageLog
{
    StageVariables start;
    StageVariables end;
    uint8_t valid;
} __attribute__((__packed__));

struct Point
{
    uint16_t x;
    union
    {
        flow_t flow;
        pressure_t pressure;
        percent_t power;
        percent_t piston_position;
        uint8_t val;
    } y;
} __attribute__((__packed__));

struct Limits
{
    pressure_t pressure;
    flow_t flow;
} __attribute__((__packed__));

struct Dynamics
{
    ControlType controlSelect : ControlType_MAX_BITS;
    InputType inputSelect : InputType_MAX_BITS;
    InterpolationType interpolation : InterpolationType_MAX_BITS;
    uint8_t points_len;
    Point *points;
    Limits limits;
} __attribute__((__packed__));

struct Stage;

struct ExitTrigger
{
    ExitType type : ExitType_MAX_BITS;
    ExitComparison comparison : ExitComparison_MAX_BITS;
    ExitReferenceType reference : ExitReferenceType_MAX_BITS;
    uint8_t target_stage : STAGES_MAX_BITS;
    uint32_t value : PROFILE_REFERENCE_MAX_BITS;
} __attribute__((__packed__));

struct Stage
{
    Dynamics dynamics;
    uint8_t exitTrigger_len;
    ExitTrigger *exitTrigger;
} __attribute__((__packed__));

struct Profile
{
    uint32_t startTime;
    uint8_t stages_len;
    StageLog *stage_log;
    Stage *stages;

    temperature_t temperature : 10;
    weight_t finalWeight : 15;

    bool wait_after_heating : 1;
    bool auto_purge : 1;

} __attribute__((__packed__));


void freeProfile(Profile* profile);

#endif