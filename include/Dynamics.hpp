//
// Created by lohse on 11/26/24.
//

#ifndef MICROPROFILEENGINE_DYNAMICS_HPP
#define MICROPROFILEENGINE_DYNAMICS_HPP

#include <cstdint>
#include <array>
#include <variant>
#include "gsl/gsl"
#include "types.hpp"

namespace profile
{
    struct ControlType
    {
        enum Type : uint8_t {
            Pressure = 0,
            Flow = 1,
            Power = 2,
            PistonPosition = 3,
        };

        static constexpr int WIDTH = log2(static_cast<int>(Type::PistonPosition)) + 1;

        operator Type() const;

    private:
        Type value;
    };
    struct InputType
    {
        enum Type : uint8_t {
            TIME = 0,
            PISTON_POSITION = 1,
            WEIGHT = 2,
        };
        static constexpr int WIDTH = log2(static_cast<int>(Type::WEIGHT)) + 1;

        operator Type() const;

    private:
        Type value;
    };

    struct Point
    {
        double x, y;
    };

    struct InterpolationAlgorithm
    {
        virtual double getValue(gsl::span<Point> points, double index, std::size_t current_index) = 0;
        virtual ~InterpolationAlgorithm() = default;
    };

    struct LinearInterpolation : public InterpolationAlgorithm
    {
        double getValue(gsl::span<Point> points, double index, std::size_t current_index) override;
        ~LinearInterpolation() override = default;
    };

    struct Limit
    {
        enum Type : uint8_t {
            Pressure = 0,
            Flow = 1,
        };

        operator Type() const;
        double operator*() const;

    private:
        Type type;
        double value;
    };

    struct SegmentIndexOrValue;

    class Dynamics
    {
        InputType inputSelect;
        std::unique_ptr<InterpolationAlgorithm> interpolation;
        std::vector<Point> points;

        std::variant<size_t, double> find_current_segment(double input) const;
        //        SegmentIndexOrValue find_current_segment(double input);
    public:
        InputType inputType() const;
        double runInterpolation(double input) const;
    };
}  // namespace profile
#endif  // MICROPROFILEENGINE_DYNAMICS_HPP
