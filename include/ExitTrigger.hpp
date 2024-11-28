#ifndef __PROFILE_EXIT_TRIGGER__
#define __PROFILE_EXIT_TRIGGER__

#include <cstdint>
#include <optional>
#include <chrono>
#include "types.hpp"
#include "ArduinoJson.h"

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

    public:
        ExitTrigger(ExitType type, ExitComparison comparison, uint8_t target_stage, uint32_t value);
        ExitType exitType() const;
        ExitComparison exitComp() const;
        std::optional<uint8_t> targetStage() const;
        uint32_t value() const;

        template <SensorState T>
        bool checkCond(Driver<T>& input, std::chrono::time_point<std::chrono::system_clock>& stageTime,
                       std::chrono::time_point<std::chrono::system_clock>& profileTime) const;
    };
}  // namespace profile
#endif