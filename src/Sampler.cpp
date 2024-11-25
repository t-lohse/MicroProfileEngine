#include "Sampler.hpp"

SamplerPoint::SamplerPoint(ControlType type, Point point, long unit_conversion_factor)
{
    switch (type) {
    case ControlType::CONTROL_PRESSURE: this->y = parseProfilePressure(point.y.pressure); break;
    case ControlType::CONTROL_FLOW: this->y = parseProfileFlow(point.y.flow); break;
    case ControlType::CONTROL_POWER: this->y = parseProfilePercent(point.y.power); break;
    case ControlType::CONTROL_PISTON_POSITION: this->y = parseProfilePercent(point.y.piston_position); break;
    }

    this->x = point.x / 10.0f * unit_conversion_factor;
}

double Sampler::get(long current_reference_input)
{
    SamplerPoint first_point = this->points.front();
    SamplerPoint last_point = this->points.back();

    if (this->points.size() == 1)
        return this->points[0].y;

    // keep the first point
    if (current_reference_input <= first_point.x)
        return first_point.y;

    // Hold the last point
    if (current_reference_input >= last_point.x)
        return last_point.y;

    switch (this->interpolation) {
    case InterpolationType::INTERPOLATION_LINEAR: return this->get_value_linear(current_reference_input);
    default: throw new UnknownInterpolationType();
    }
}

void Sampler::load_new_stage(const Stage* stage, int16_t stageId)
{
    ControlType type = stage->dynamics.controlSelect;
    InterpolationType interpolation = stage->dynamics.interpolation;
    long unit_conversion_factor = stage->dynamics.inputSelect == InputType::INPUT_TIME ? 1000 : 1;

    std::vector<Point> points(stage->dynamics.points, stage->dynamics.points + stage->dynamics.points_len);

    for (Point point : points) {
        printf("InputPoint: (%f:%f)\n", point.x / 10.0f, point.y.val / 10.0f);
    }

    printf("Loading stage %d with %ld points into the sampler \n", stageId, points.size());

    this->load_new_points(type, points, unit_conversion_factor, interpolation);

    for (SamplerPoint point : this->points) {
        printf("SamplerPoint: (%f:%f)\n", point.x, point.y);
    }
    this->stageId = stageId;
}

void Sampler::load_new_points(ControlType current_control, std::vector<Point>& points, long unit_conversion_factor,
                              InterpolationType interpolation)
{
    this->interpolation = interpolation;
    this->time_series_index = 0;
    this->points.clear();
    for (Point& point : points) {
        SamplerPoint p(current_control, point, unit_conversion_factor);
        this->points.push_back(p);
    }
}

void Sampler::find_current_segment(long current_value)
{
    while (current_value > this->current_segment_end) {
        this->current_segment_start = this->current_segment_end;
        this->time_series_index++;
        if (static_cast<size_t>(time_series_index) >= this->points.size()) {
            this->current_segment_end = UINT64_MAX;
            return;
        }
        this->current_segment_end = (this->points[this->time_series_index].x);
    }

    while (current_value < this->current_segment_start) {
        this->current_segment_end = this->current_segment_start;
        this->time_series_index++;
        if (time_series_index < 0) {
            this->current_segment_start = 0;
            return;
        }
        this->current_segment_start = (this->points[this->time_series_index].x);
    }
}

double Sampler::get_value_linear(long current_reference_input)
{
    find_current_segment(current_reference_input);

    // keep the first point
    if (this->time_series_index < 0)
        return this->points.front().y;

    // Hold the last point
    if (static_cast<size_t>(time_series_index) > this->points.size())
        return this->points.back().y;

    // Do the actual interpolation
    double slope = (this->points[this->time_series_index].y - this->points[this->time_series_index - 1].y) /
                   ((this->points[this->time_series_index].x) - (this->points[this->time_series_index - 1].x));

    double intercept = this->points[this->time_series_index].y - slope * (this->points[this->time_series_index].x);
    return (slope * current_reference_input) + intercept;
}
