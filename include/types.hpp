//
// Created by lohse on 11/26/24.
//

#ifndef MICROPROFILEENGINE_TYPES_HPP
#define MICROPROFILEENGINE_TYPES_HPP
#include <cstdint>
#include <string>
#include <variant>

namespace profile
{
    class flow_t
    {
        double val;

    public:
        flow_t() = delete;

        [[maybe_unused]] flow_t(double v);

        [[maybe_unused]] double get() const;
        operator double() const;
        [[maybe_unused]] double getRaw() const;
    };
    class pressure_t
    {
        double val;

    public:
        pressure_t() = delete;
        [[maybe_unused]] pressure_t(double v);
        operator double() const;
        [[maybe_unused]] double get() const;
        [[maybe_unused]] double getRaw() const;
    };
    class percent_t
    {
        double val;

    public:
        percent_t() = delete;
        [[maybe_unused]] percent_t(double v);
        operator double() const;
        [[maybe_unused]] double get() const;
        [[maybe_unused]] double getRaw() const;
    };
    class temperature_t
    {
        double val;

    public:
        temperature_t() = delete;

        [[maybe_unused]] temperature_t(double v);
        operator double() const;
        [[maybe_unused]] double get() const;
        [[maybe_unused]] double getRaw() const;
    };
    class weight_t
    {
        double val;

    public:
        weight_t() = delete;

        [[maybe_unused]] weight_t(double v);
        operator double() const;
        [[maybe_unused]] double get() const;
        [[maybe_unused]] double getRaw() const;
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

        static ProfileError nameError(std::string&& s) { return ProfileError(TypeErr::Name, std::move(s)); }
        static ProfileError typeError(std::string&& s) { return ProfileError(TypeErr::Type, std::move(s)); }
        static ProfileError parseError(std::string&& s) { return ProfileError(TypeErr::JsonParsing, std::move(s)); }
        static ProfileError noName(std::string&& s)
        {
            return ProfileError(TypeErr::Name, "Expected entry named `" + std::move(s) + "`, could not find");
        }
        static ProfileError unexpectedType(std::string&& s)
        {
            return ProfileError(TypeErr::Name, "Expected type `" + std::move(s) + "`, got something else");
        }

    private:
        using ErrType = std::variant<std::string>;
        ProfileError(TypeErr t, ErrType v): type{t}, value{v} {};
        TypeErr type;
        ErrType value;
    };
}  // namespace profile

#endif  // MICROPROFILEENGINE_TYPES_HPP
