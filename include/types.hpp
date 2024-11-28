//
// Created by lohse on 11/26/24.
//

#ifndef MICROPROFILEENGINE_TYPES_HPP
#define MICROPROFILEENGINE_TYPES_HPP
#include <cstdint>
namespace profile {
    class flow_t {double val;
    public:
    flow_t() = delete;
    flow_t(double v) : val{v} {};
    double get() const;
    double getRaw() const;
    };
    class pressure_t {double val;
    public:
        pressure_t() = delete;
        pressure_t(double v);
        double get() const;
        double getRaw() const;
    };
    class percent_t {double val;
    public:
        percent_t() = delete;
        percent_t(double v);
        double get() const;
        double getRaw() const;
    };
    class temperature_t {double val;
    public:
        temperature_t() = delete;
        temperature_t(double v);
        double get() const;
        double getRaw() const;
    };
    class weight_t {double val;
    public:
        weight_t() = delete;
        weight_t(double v);
        double get() const;
        double getRaw() const;
    };

    static constexpr int log2(int index) {
        int i = 0;
        while (index >>= 1) ++i;
        return i;
    }
}

#endif //MICROPROFILEENGINE_TYPES_HPP
