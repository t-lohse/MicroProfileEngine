#ifndef __SAMPLER_H__
#define __SAMPLER_H__

#include "ProfileDefinition.h"
#include <array>
#include <vector>

struct UnknownInterpolationType : std::exception
{
};

class SamplerPoint
{
public:
    SamplerPoint() : x(0), y(0) {}
    explicit SamplerPoint(ControlType type, Point point, long unit_conversion_factor);
    explicit SamplerPoint(double x, double y) : x(x), y(y) {}
    double x;
    double y;
};

class Sampler
{
public:
    Sampler() {}

    double get(long current_reference_input);
    void load_new_stage(const Stage *stage, int16_t stage_id);
    void load_new_points(
        ControlType current_control,
        std::vector<Point> &points,
        long unit_conversion_factor,
        InterpolationType interpolation);
    uint16_t stageId = -1;

private:
    int32_t time_series_index = 0;
    long current_segment_start = 0;
    long current_segment_end = 0;

    std::vector<SamplerPoint> points;
    InterpolationType interpolation;

    void find_current_segment(long current_value);
    double get_value_linear(long current_value);
};

#endif