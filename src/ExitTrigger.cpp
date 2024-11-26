
#include "ExitTrigger.hpp"
//#include "cmath"

namespace profile {

    static constexpr int pow(int operand, int exponant) {
        while (exponant-- > 0)
            operand *= operand;

        return operand;
    }

    ExitComparison::ExitComparison(ExitComparison::Comp v) : value{v} {}
    ExitType::ExitType(ExitType::Type v) : value{v} {}
    ExitTrigger::ExitTrigger(profile::ExitType type, profile::ExitComparison comparison, uint8_t target_stage,
                             uint32_t value) : _value(value << VALUE_OFFSET |
                                                      (type.value << TYPE_OFFSET) |
                                                      (comparison.value << COMP_OFFSET) |
                                                      (target_stage << TARGET_OFFSET)) {
        // TODO: assert
        //static_assert(value >> EXITTRIGGER_SIZE == 0);
    }
    ExitType ExitTrigger::exitType() {
        return ExitType(static_cast<ExitType::Type>(((_value >> TYPE_OFFSET)) & (pow(2, ExitType::WIDTH) - 1)));

    }
    ExitComparison ExitTrigger::exitComp() {
        return ExitComparison(static_cast<ExitComparison::Comp>(((_value >> COMP_OFFSET)) & (pow(2, ExitComparison::WIDTH) - 1)));
    }
    std::optional<uint8_t> ExitTrigger::targetStage() {
        auto v = ((_value >> TARGET_OFFSET)) & (pow(2, UINT8_WIDTH) - 1);
        return v == 0 ? std::optional<uint8_t>{} : v;
    }
    uint32_t ExitTrigger::value() {
        return ((_value >> VALUE_OFFSET)) & (pow(2, UINT32_WIDTH) - 1);
    }

    template<typename T>
    bool ExitTrigger::checkCond(Driver<T> input, std::chrono::system_clock &stageTime, std::chrono::system_clock &profileTime){
        return true;
    }
}