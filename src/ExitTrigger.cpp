
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
        _value(value << VALUE_OFFSET | (static_cast<uint8_t>(type.value) << TYPE_OFFSET) |
               (static_cast<uint8_t>(comparison.value) << COMP_OFFSET) | (target_stage << TARGET_OFFSET))
    {
        // TODO: assert
        // static_assert(value >> EXITTRIGGER_SIZE == 0);
    }
    ExitType ExitTrigger::exitType() const
    {
        return ExitType(static_cast<ExitType::Type>(((_value >> TYPE_OFFSET)) & (pow(2, ExitType::WIDTH) - 1)));
    }
    ExitType::operator Type() const { return value; }
    ExitComparison ExitTrigger::exitComp() const
    {
        return ExitComparison(
            static_cast<ExitComparison::Comp>(((_value >> COMP_OFFSET)) & (pow(2, ExitComparison::WIDTH) - 1)));
    }
    std::optional<uint8_t> ExitTrigger::targetStage() const
    {
        auto v = ((_value >> TARGET_OFFSET)) & (pow(2, UINT8_WIDTH) - 1);
        return v > 0 ? std::optional<uint8_t>{} : v;
    }
    uint32_t ExitTrigger::value() const { return ((_value >> VALUE_OFFSET)) & (pow(2, UINT32_WIDTH) - 1); }

}  // namespace profile
