//
// Created by lohse on 11/26/24.
//
#include <algorithm>

#include "Dynamics.hpp"
namespace profile
{
    double LinearInterpolation::getValue(std::vector<Point> points, double input, std::size_t currentIndex)
    {
        double slope = (points[currentIndex].y - points[currentIndex - 1].y) /
                       (points[currentIndex].x - points[currentIndex - 1].y);
        auto intercept = points[currentIndex].y - slope * points[currentIndex].x;

        return (slope * input) + intercept;
    }

    std::variant<size_t, double> Dynamics::find_current_segment(double input) const
    {
        if (auto v = points.front(); points.size() == 1 || points.front().x >= input)
            return v.y;
        else if (v = points.back(); v.x <= input)
            return v.y;
        auto predicate = [=](auto p) { return p.x >= input; };

        return (*std::find_if(points.begin(), points.end(), predicate)).y;
    }
    InputType Dynamics::inputType() const { return inputSelect; }

    InputType::operator Type() const { return value; }

    ControlType::operator Type() const { return value; }

    Limit::operator Type() const { return type; }
    double Limit::operator*() const { return value; }

    template <class Tsize, class Tval>
    struct V : Tsize, Tval
    {
        using Tsize::operator();
        using Tval::operator();
    };
    double Dynamics::runInterpolation(double input) const
    {
        auto v = V{[&](size_t v) { return interpolation->getValue(points, input, v); }, [](double v) { return v; }};
        return std::visit(v, find_current_segment(input));
    }
}  // namespace profile