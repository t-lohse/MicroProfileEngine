//
// Created by lohse on 11/28/24.
//
#include "types.hpp"
namespace profile
{

    const double INTERNAL_VALUE_SCALAR = 10.0;

    flow_t::flow_t(double v): val{v * INTERNAL_VALUE_SCALAR} {}
    flow_t::operator double() const { return val / INTERNAL_VALUE_SCALAR; }
    double flow_t::get() const { return *this; }
    double flow_t::getRaw() const { return val; }

    pressure_t::pressure_t(double v): val{v * INTERNAL_VALUE_SCALAR} {}
    pressure_t::operator double() const { return val / INTERNAL_VALUE_SCALAR; }
    double pressure_t::get() const { return *this; }
    double pressure_t::getRaw() const { return val; }

    percent_t::percent_t(double v): val{v * INTERNAL_VALUE_SCALAR} {}
    percent_t::operator double() const { return val / INTERNAL_VALUE_SCALAR; }
    double percent_t::get() const { return *this; }
    double percent_t::getRaw() const { return val; }

    temperature_t::temperature_t(double v): val{v * INTERNAL_VALUE_SCALAR} {}
    temperature_t::operator double() const { return val / INTERNAL_VALUE_SCALAR; }
    double temperature_t::get() const { return *this; }
    double temperature_t::getRaw() const { return val; }

    weight_t::weight_t(double v): val{v * INTERNAL_VALUE_SCALAR} {}
    weight_t::operator double() const { return val / INTERNAL_VALUE_SCALAR; }
    double weight_t::get() const { return *this; }
    double weight_t::getRaw() const { return val; }

}  // namespace profile