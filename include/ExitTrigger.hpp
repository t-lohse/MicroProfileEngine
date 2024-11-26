#ifndef __PROFILE_EXIT_TRIGGER__
#define __PROFILE_EXIT_TRIGGER__

#include <cstdint>
#include <optional>
#include <chrono>


// TODO: Concept, or dispatch?
template <typename>
struct Driver;

namespace profile {
    static constexpr int log2(int index) {
        int i = 0;
        while (index >>= 1) ++i;
        return i;
    }

    class ExitTrigger;
    struct ExitType {
        enum Type : uint8_t {
            Pressure = 0,
            FLOW = 1,
            TIME_RELATIVE = 2,
            TIME_ABSOLUTE = 3,
            WEIGHT = 4,
            PISTON_POSITION = 5,
            POWER = 6,
            TEMPERATURE = 7,
            BUTTON = 8,
        };
        static constexpr int WIDTH =
             log2(static_cast<int>(ExitType::BUTTON)) + 1;


    private:
        ExitType(Type v);
        Type value : WIDTH;
        friend class ExitTrigger;
    };

    struct ExitComparison {
        enum Comp : uint8_t {
            SMALLER = 0,
            SMALLER_STRICT = 1,
            GREATER = 2,
            GREATER_STRICT = 3,
        };

        static constexpr int WIDTH =
                log2(static_cast<int>(Comp::GREATER_STRICT)) + 1;
    private:
        ExitComparison(Comp v);
        Comp value : WIDTH;
        friend class ExitTrigger;
    };

    class ExitTrigger {
    private:

        //ExitType _type : ExitType::WIDTH;
        //ExitComparison _comparison : ExitComparison::WIDTH;
        //uint8_t _target_stage;
        uint32_t _value;
        static constexpr int TYPE_OFFSET = 0;
        static constexpr int COMP_OFFSET = ExitType::WIDTH;
        static constexpr int TARGET_OFFSET = COMP_OFFSET + ExitComparison::WIDTH;
        static constexpr int VALUE_OFFSET = TARGET_OFFSET + UINT8_WIDTH;
    public:
        ExitTrigger(ExitType type, ExitComparison comparison, uint8_t target_stage, uint32_t value);
        ExitType exitType();
        ExitComparison exitComp();
        std::optional<uint8_t> targetStage();
        uint32_t value();

        template<typename T>
        bool checkCond(Driver<T> input, std::chrono::system_clock &stageTime, std::chrono::system_clock &profileTime);
    };
}
#endif