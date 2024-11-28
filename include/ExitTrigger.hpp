#ifndef __PROFILE_EXIT_TRIGGER__
#define __PROFILE_EXIT_TRIGGER__

#include <cstdint>
#include <optional>
#include <chrono>
#include "types.hpp"
#include "ArduinoJson.h"
#include "expected"
#include "iostream"
#include "vector"

#include "sensor.hpp"

namespace profile
{
    class ExitTrigger;
    struct ExitType
    {
        enum Type : uint8_t {
            Pressure = 0,
            Flow = 1,
            TimeRelative = 2,
            TimeAbsolute = 3,
            Weight = 4,
            PistonPosition = 5,
            Power = 6,
            Temperature = 7,
            Button = 8,
        };
        static constexpr int WIDTH = log2(static_cast<int>(ExitType::Button)) + 1;

        operator Type() const;

        static std::expected<ExitType, ProfileError> fromJson(ArduinoJson::JsonObject& obj) {
            std::string t;
            {
                auto _t = obj["type"];
                if (!_t) {
                    return std::unexpected(ProfileError::nameError("type"));
                }
                t = _t.as<std::string>();
            }

            if (t == "pressure") {
                return ExitType(Type::Pressure);
            } if (t == "flow") {
                return ExitType(Type::Flow);
            } if (t == "weight") {
                return ExitType(Type::Weight);
            } if (t == "piston_position") {
                return ExitType(Type::PistonPosition);
            } if (t == "power") {
                return ExitType(Type::Power);
            }if (t == "temperature") {
                return ExitType(Type::Temperature);
            }if (t == "button") {
                return ExitType(Type::Button);
            } if (t == "time") {
                if (obj["relative"].as<bool>())
                    return ExitType(Type::TimeRelative);
                return ExitType(Type::TimeAbsolute);
            }
            return std::unexpected(ProfileError::noName("No valid value for type"));
        }

    private:
        ExitType(Type v);
        Type value : WIDTH;
        friend class ExitTrigger;
    };

    struct ExitComparison
    {
        enum Comp : uint8_t {
            Smaller = 0,
            SmallerStrict = 1,
            Greater = 2,
            GreaterStrict = 3,
        };

        static constexpr int WIDTH = log2(static_cast<int>(Comp::GreaterStrict)) + 1;

        static std::expected<ExitComparison, ProfileError> fromJson(ArduinoJson::JsonObject& obj) {
            std::string t;
            {
                auto _t = obj["comparison"];
                if (!_t) {
                    return std::unexpected(ProfileError::nameError("comparison"));
                }
                t = _t.as<std::string>();
            }

            if (t == "greater-strict" || t == ">") {
                return ExitComparison(Comp::GreaterStrict);
            } if (t == "greater" || t == ">=") {
                return ExitComparison(Comp::Greater);
            } if (t == "smaller-strict" || t == "<") {
                return ExitComparison(Comp::SmallerStrict);
            } if (t == "smaller" || t == "<=") {
                return ExitComparison(Comp::Smaller);
            }
            return std::unexpected(ProfileError::noName("No valid value for type"));
        }

        template <typename T>
        bool comp(T lhs, T rhs) const
        {
            switch (value) {
            case Comp::Smaller: return lhs <= rhs;
            case Comp::SmallerStrict: return lhs < rhs;
            case Comp::Greater: return lhs >= rhs;
            case Comp::GreaterStrict: return lhs > rhs;
            }
        }



    private:
        ExitComparison(Comp v);
        Comp value : WIDTH;
        friend class ExitTrigger;
    };

    class ExitTrigger
    {
    private:
        // ExitType _type : ExitType::WIDTH;
        // ExitComparison _comparison : ExitComparison::WIDTH;
        // uint8_t _target_stage;
        uint32_t _value;
        static constexpr int TYPE_OFFSET = 0;
        static constexpr int COMP_OFFSET = ExitType::WIDTH;
        static constexpr int TARGET_OFFSET = COMP_OFFSET + ExitComparison::WIDTH;
        static constexpr int VALUE_OFFSET = TARGET_OFFSET + UINT8_WIDTH;

        ExitTrigger(ExitType type, ExitComparison comparison, uint8_t target_stage, uint32_t value);

    public:
        ExitType exitType() const;
        ExitComparison exitComp() const;
        std::optional<uint8_t> targetStage() const;
        uint32_t value() const;

        static std::expected<ExitTrigger, ProfileError> fromJson(ArduinoJson::JsonObject&& a) {
            auto et = ExitType::fromJson(a);
            if (!et)
                return std::unexpected(et.error());
            ExitType type = et.value();
            auto comp = ExitComparison::fromJson(a).value_or(ExitComparison(ExitComparison::Greater));

            uint8_t targetStage = a["target_stage"] | 0;

            auto ev = a["value"];
            if (!ev)
                return std::unexpected(ProfileError::noName("value"));
            uint32_t value = ev.as<uint32_t >();
            return ExitTrigger(type, comp, targetStage, value);
        }
        static std::expected<std::vector<ExitTrigger>, ProfileError> fromJson(ArduinoJson::JsonArray&& obj) {
            if (!obj)
                return std::unexpected(ProfileError::noName("exit_triggers"));
            if (obj.size() <= 0)
                return std::unexpected(ProfileError::parseError("No elements in `exit_triggers` found"));
            std::vector<ExitTrigger> out{};
            out.reserve(obj.size());
            for (ArduinoJson::JsonObject&& a : obj) {
                auto e = ExitTrigger::fromJson(std::move(a));
                if (!e)
                    return std::unexpected(e.error());
                out.emplace_back(e.value());
            }

            return out;
        }

        template <SensorState T>
        bool checkCond(Driver<T>& input, std::chrono::time_point<std::chrono::system_clock>& stageTime,
                       std::chrono::time_point<std::chrono::system_clock>& profileTime) const;
    };
}  // namespace profile
#endif