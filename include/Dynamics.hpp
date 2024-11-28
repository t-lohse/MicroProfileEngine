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
#include "ArduinoJson.h"
#include "expected"
#include <iostream>

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

        static std::expected<ControlType, ProfileError> fromJson(const ArduinoJson::JsonObject& a) {
            auto ev = a["type"];
            if (!ev)
                return std::unexpected(ProfileError::noName("over"));
            if (!ev.is<std::string>())
                return std::unexpected(ProfileError::typeError("string"));
            auto typ = ev.as<std::string>();

            if (typ == "pressure" ) {
                return ControlType(Type::Pressure);
            } if (typ == "flow" ) {
                return ControlType(Type::Flow);
            } if (typ == "power" ) {
                return ControlType(Type::Power);
            }
            if (typ == "piston_position" ) {
                return ControlType(Type::PistonPosition);
            }
            return std::unexpected(ProfileError::noName("No valid value for field `over`"));
        }

        operator Type() const;

    private:
        explicit ControlType(Type t) : value{t}{}
        Type value;
    };
    struct InputType
    {
        enum Type : uint8_t {
            Time = 0,
            PistonPosition = 1,
            Weight = 2,
        };
        static constexpr int WIDTH = log2(static_cast<int>(Type::Weight)) + 1;

        operator Type() const;

        static std::expected<InputType, ProfileError> fromJson(const ArduinoJson::JsonObject& a) {
            auto ev = a["over"];
            if (!ev)
                return std::unexpected(ProfileError::noName("over"));
            if (!ev.is<std::string>())
                return std::unexpected(ProfileError::typeError("string"));
            auto typ = ev.as<std::string>();

            if (typ == "time" ) {
                return InputType(Type::Time);
            } if (typ == "piston_position" ) {
                return InputType(Type::PistonPosition);
            }
            if (typ == "weight" ) {
                return InputType(Type::Weight);
            }
            return std::unexpected(ProfileError::noName("No valid value for field `over`"));
        }


    private:
        explicit InputType(Type t) : value{t}{}
        Type value;
    };

    struct Point
    {
        double x, y;

        static std::expected<std::vector<Point>, ProfileError> fromJson(const ArduinoJson::JsonObject& a) {
            // auto e = static_cast<JsonObject>(a);
            JsonArray p = a["points"];
            if (!p)
                return std::unexpected(ProfileError::noName("points"));
            if (p.size() <= 0)
                return std::unexpected(ProfileError::parseError("No points provided"));

            std::vector<Point> out{};
            out.reserve(p.size());
            for (JsonArray o : p) {
                auto _p = Point::fromJson(std::move(o));
                if (!_p)
                    return std::unexpected(_p.error());
                out.emplace_back(_p.value());

            }
            return out;
        }
        static std::expected<Point, ProfileError> fromJson(ArduinoJson::JsonArray&& obj) {
            if (!obj)
                return std::unexpected(ProfileError::typeError("what"));
            if (obj.size() != 2)
                return std::unexpected(ProfileError::parseError("Wrong sized points, only 2 items allowed"));

            if (!obj[0].is<double>())
                return std::unexpected(ProfileError::enexpectedType("double"));
            if (!obj[1].is<double>())
                return std::unexpected(ProfileError::enexpectedType("double"));

            return Point(obj[0].as<double>(),obj[1].as<double>());
        }

    private:
        Point(double x_, double y_) : x(x_), y(y_) {}
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

    static std::expected<std::unique_ptr<InterpolationAlgorithm>, ProfileError> fromJsonInterpolation(const ArduinoJson::JsonObject& a) {
        auto ev = a["interpolation"];
        if (!ev)
            return std::unexpected(ProfileError::noName("interpolation"));
        if (!ev.is<std::string>())
            return std::unexpected(ProfileError::typeError("string"));
        auto typ = ev.as<std::string>();

        if (typ == "linear" ) {
            return std::unique_ptr<InterpolationAlgorithm>(new LinearInterpolation());
        }
        return std::unexpected(ProfileError::typeError("only `linear` interpolation supported"));
    }

    struct Limit
    {
        enum Type : uint8_t {
            Pressure = 0,
            Flow = 1,
        };

        operator Type() const;
        double operator*() const;

        static std::expected<Limit, ProfileError> fromJson(ArduinoJson::JsonObject& a) {
           // auto e = static_cast<JsonObject>(a);
           auto ev = a["value"];
            if (!ev)
                return std::unexpected(ProfileError::noName("value"));
            if (!ev.is<double>())
                return std::unexpected(ProfileError::typeError("double"));
            double val = ev.as<double>();

            auto et = a["type"];
            if (!et)
                return std::unexpected(ProfileError::noName("type"));
            if (!et.is<std::string>())
                return std::unexpected(ProfileError::typeError("string"));
            auto typ = et.as<std::string>();

            if (typ == "flow" ) {
                return Limit(Type::Flow, val);
            } if (typ == "pressure" ) {
                return Limit(Type::Pressure, val);
            }
            return std::unexpected(ProfileError::noName("No valid value for field `type`"));
        }
        static std::expected<std::vector<Limit>, ProfileError> fromJson(ArduinoJson::JsonArray&& obj) {
            if (!obj)
                return std::unexpected(ProfileError::noName("limits"));
            if (obj.size() <= 0)
                return std::unexpected(ProfileError::parseError("No elements in `limits` found"));
            std::vector<Limit> out{};
            out.reserve(obj.size());
            for (ArduinoJson::JsonObject&& a : obj) {
                auto e = Limit::fromJson(a);
                if (!e)
                    return std::unexpected(e.error());
                out.emplace_back(e.value());
            }

            return out;
        }

    private:
        explicit Limit(Type t, double v) : type(t), value(v) {}
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
        explicit Dynamics(
                std::vector<Point> points,
                std::unique_ptr<InterpolationAlgorithm>&& interpolation,
                InputType inputSelect
        ) : points(points), interpolation(std::move(interpolation)), inputSelect(inputSelect) {}

    public:
        InputType inputType() const;
        double runInterpolation(double input) const;

        static std::expected<Dynamics, ProfileError> fromJson(const ArduinoJson::JsonObject& a) {
            // auto e = static_cast<JsonObject>(a);
            if (!a)
                return std::unexpected(ProfileError::noName("dynamics"));

            auto points = Point::fromJson(a);
            if (!points)
                return std::unexpected(points.error());
            auto interpolation = fromJsonInterpolation(a);
            if (!interpolation)
                return std::unexpected(interpolation.error());
            auto inputSelect = InputType::fromJson(a);
            if (!inputSelect)
                return std::unexpected(inputSelect.error());
            return Dynamics(points.value(), std::move(interpolation.value()), inputSelect.value());
        }
    };
}  // namespace profile
#endif  // MICROPROFILEENGINE_DYNAMICS_HPP
