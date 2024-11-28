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

namespace profile {
    struct ControlType {
        enum Type : uint8_t {
            PRESSURE = 0,
            FLOW = 1,
            POWER = 2,
            PISTON_POSITION = 3,
        };

        static constexpr int WIDTH = log2(static_cast<int>(Type::PISTON_POSITION)) + 1;

    private:
        Type value;
    };
    struct InputType {
        enum Type : uint8_t {
            TIME = 0,
            PISTON_POSITION = 1,
            WEIGHT = 2,
        };
        static constexpr int WIDTH = log2(static_cast<int>(Type::WEIGHT)) + 1;

    private:
        Type value;
    };

    struct Point {
        double x, y;
    };

    struct InterpolationAlgorithm {
       virtual double getValue(gsl::span<Point> points, double index, std::size_t current_index) = 0;
       virtual ~InterpolationAlgorithm() = default;
    };

    struct LinearInterpolation : public InterpolationAlgorithm {
        double getValue(gsl::span<Point> points, double index, std::size_t current_index) override;
        ~LinearInterpolation() override = default;
    };

    struct Limit {
        enum Type : uint8_t {
            Pressure = 0,
            Flow = 1,
        };
    private:
        Type type;
        double value;
    };

    struct SegmentIndexOrValue;

    class Dynamics {
        InputType inputSelect;
        std::unique_ptr<InterpolationAlgorithm> interpolation;
        std::vector<Point> points;

        std::variant<size_t, double> find_current_segment(double input);
//        SegmentIndexOrValue find_current_segment(double input);
    public:
        //Dynamics(Dynamics&) = default;

        InputType inputType();
        double runInterpolation(double input);

    };
}
#endif //MICROPROFILEENGINE_DYNAMICS_HPP
