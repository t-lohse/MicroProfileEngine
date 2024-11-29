//
// Created by lohse on 11/26/24.
//

#ifndef MICROPROFILEENGINE_TYPES_HPP
#define MICROPROFILEENGINE_TYPES_HPP
#include <cstdint>
#include "string"
#include "variant"

namespace profile
{
    class flow_t
    {
        double val;

    public:
        flow_t() = delete;
        flow_t(double v);
        double get() const;
        operator double() const;
        double getRaw() const;
    };
    class pressure_t
    {
        double val;

    public:
        pressure_t() = delete;
        pressure_t(double v);
        operator double() const;
        double get() const;
        double getRaw() const;
    };
    class percent_t
    {
        double val;

    public:
        percent_t() = delete;
        percent_t(double v);
        operator double() const;
        double get() const;
        double getRaw() const;
    };
    class temperature_t
    {
        double val;

    public:
        temperature_t() = delete;
        temperature_t(double v);
        operator double() const;
        double get() const;
        double getRaw() const;
    };
    class weight_t
    {
        double val;

    public:
        weight_t() = delete;
        weight_t(double v);
        operator double() const;
        double get() const;
        double getRaw() const;
    };

    static constexpr int log2(int index)
    {
        int i = 0;
        while (index >>= 1)
            ++i;
        return i;
    }

    struct ProfileError
    {
        enum class TypeErr : uint8_t { Name, Type, JsonParsing };

        operator TypeErr() const { return type; }

        static ProfileError nameError(std::string&& s) { return ProfileError(TypeErr::Name, s); }
        static ProfileError typeError(std::string&& s) { return ProfileError(TypeErr::Type, s); }
        static ProfileError parseError(std::string&& s) { return ProfileError(TypeErr::JsonParsing, s); }
        static ProfileError noName(std::string&& s)
        {
            return ProfileError(TypeErr::Name, "Expected entry named `" + s + "`, could not find");
        }
        static ProfileError enexpectedType(std::string&& s)
        {
            return ProfileError(TypeErr::Name, "Expected type `" + s + "`, got something else");
        }

    private:
        using ValType = std::variant<std::string>;
        ProfileError(TypeErr t, ValType v): type(t), value(v) {};
        TypeErr type;
        ValType value;
    };
}  // namespace profile

#endif  // MICROPROFILEENGINE_TYPES_HPP
