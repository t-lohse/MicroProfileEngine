#include "ExitTrigger.hpp"

namespace profile
{

    static constexpr uint32_t pow2(uint32_t exponent)
    {
        uint32_t operand = 1;
        for (; exponent > 0; exponent--)
            operand *= 2;

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
        return ExitType(static_cast<ExitType::Type>(((_value >> TYPE_OFFSET)) & (pow2(ExitType::WIDTH) - 1)));
    }
    ExitType::operator Type() const { return value; }
    ExitComparison ExitTrigger::exitComp() const
    {
        return ExitComparison(
            static_cast<ExitComparison::Comp>(((_value >> COMP_OFFSET)) & (pow2(ExitComparison::WIDTH) - 1)));
    }
    std::optional<uint8_t> ExitTrigger::targetStage() const
    {
        auto v = (_value >> TARGET_OFFSET) & (pow2(UINT8_WIDTH) - 1);
        return v == 0 ? std::optional<uint8_t>{} : v - 1;
    }
    uint32_t ExitTrigger::value() const { return ((_value >> VALUE_OFFSET)) & (pow2(UINT32_WIDTH) - 1); }

}  // namespace profile
