
#include "ExitTrigger.hpp"
#include "limits"
// #include "cmath"

namespace profile
{

    static constexpr int pow(int operand, int exponant)
    {
        while (exponant-- > 0)
            operand *= operand;

        return operand;
    }

    // Comp
    ExitComparison::ExitComparison(ExitComparison::Comp v): value{v} {}

    // Type
    ExitType::ExitType(ExitType::Type v): value{v} {}

    // Trigger
    ExitTrigger::ExitTrigger(profile::ExitType type, profile::ExitComparison comparison, uint8_t target_stage,
                             uint32_t value):
        _value(value << VALUE_OFFSET | (type.value << TYPE_OFFSET) | (comparison.value << COMP_OFFSET) |
               (target_stage << TARGET_OFFSET))
    {
        // TODO: assert
        // static_assert(value >> EXITTRIGGER_SIZE == 0);
    }
    ExitType ExitTrigger::exitType() const
    {
        return ExitType(static_cast<ExitType::Type>(((_value >> TYPE_OFFSET)) & (pow(2, ExitType::WIDTH) - 1)));
    }
    ExitComparison ExitTrigger::exitComp() const
    {
        return ExitComparison(
            static_cast<ExitComparison::Comp>(((_value >> COMP_OFFSET)) & (pow(2, ExitComparison::WIDTH) - 1)));
    }
    std::optional<uint8_t> ExitTrigger::targetStage() const
    {
        auto v = ((_value >> TARGET_OFFSET)) & (pow(2, UINT8_WIDTH) - 1);
        return v == 0 ? std::optional<uint8_t>{} : v;
    }
    uint32_t ExitTrigger::value() const { return ((_value >> VALUE_OFFSET)) & (pow(2, UINT32_WIDTH) - 1); }

    template <SensorState T>
    bool ExitTrigger::checkCond(Driver<T>& input, std::chrono::time_point<std::chrono::system_clock>& stageTime,
                                std::chrono::time_point<std::chrono::system_clock>& profileTime) const
    {
        using ET = ExitType;
        double lhs = NAN;
        switch (exitType()) {
        case ET::Pressure: lhs = input.getSensorState().waterPressure(); break;
        case ET::Flow: lhs = input.getSensorState().waterFlow(); break;
        case ET::TimeRelative: {
            auto dur = std::chrono::duration<double>(stageTime - std::chrono::system_clock::now());
            lhs = dur.count();
            break;
        }
        case ET::TimeAbsolute: {
            auto dur = std::chrono::duration<double>(profileTime - std::chrono::system_clock::now());
            lhs = dur.count();
            break;
        }
        case ET::Weight: lhs = input.getSensorState().weight(); break;
        case ET::PistonPosition: lhs = input.getSensorState().pistonPosition(); break;
        case ET::Power: std::abort();
        case ET::Temperature: lhs = input.getSensorState().stabelTemperature(); break;
        case ET::Button:
            lhs = input.getButtonGesture() ? std::numeric_limits<double>::max() : std::numeric_limits<double>::min();
            break;
        }
        double rhs = value();
        exitComp().comp(lhs, rhs);
        return true;
    }
}  // namespace profile