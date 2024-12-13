//
// Created by lohse on 11/28/24.
//
#include "types.hpp"
namespace profile
{

    const double INTERNAL_VALUE_SCALAR = 10.0;

    [[maybe_unused]] flow_t::flow_t(double v): val{v * INTERNAL_VALUE_SCALAR} {}
    flow_t::operator double() const { return val / INTERNAL_VALUE_SCALAR; }

    [[maybe_unused]] double flow_t::get() const { return *this; }
    [[maybe_unused]] double flow_t::getRaw() const { return val; }

    [[maybe_unused]] pressure_t::pressure_t(double v): val{v * INTERNAL_VALUE_SCALAR} {}
    pressure_t::operator double() const { return val / INTERNAL_VALUE_SCALAR; }
    [[maybe_unused]] double pressure_t::get() const { return *this; }
    [[maybe_unused]] double pressure_t::getRaw() const { return val; }

    [[maybe_unused]] percent_t::percent_t(double v): val{v * INTERNAL_VALUE_SCALAR} {}
    percent_t::operator double() const { return val / INTERNAL_VALUE_SCALAR; }
    [[maybe_unused]] double percent_t::get() const { return *this; }
    [[maybe_unused]] double percent_t::getRaw() const { return val; }

    [[maybe_unused]] temperature_t::temperature_t(double v): val{v * INTERNAL_VALUE_SCALAR} {}
    temperature_t::operator double() const { return val / INTERNAL_VALUE_SCALAR; }
    [[maybe_unused]] double temperature_t::get() const { return *this; }
    [[maybe_unused]] double temperature_t::getRaw() const { return val; }

    [[maybe_unused]] weight_t::weight_t(double v): val{v * INTERNAL_VALUE_SCALAR} {}
    weight_t::operator double() const { return val / INTERNAL_VALUE_SCALAR; }
    [[maybe_unused]] double weight_t::get() const { return *this; }
    [[maybe_unused]] double weight_t::getRaw() const { return val; }

}  // namespace profile