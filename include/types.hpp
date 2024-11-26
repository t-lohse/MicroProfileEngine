//
// Created by lohse on 11/26/24.
//

#ifndef MICROPROFILEENGINE_TYPES_HPP
#define MICROPROFILEENGINE_TYPES_HPP
#include <cstdint>
namespace profile {
    typedef uint8_t flow_t;
    typedef uint8_t pressure_t;
    typedef uint8_t percent_t;
    typedef uint16_t temperature_t;
    typedef uint16_t weight_t;
    typedef uint16_t timestamp_t;


    double parseProfileFlow(flow_t flow);
    double parseProfilePressure(pressure_t pressure);
    double parseProfilePercent(percent_t percent);
    double parseProfileTemperature(temperature_t temperature);
    double parseProfileWeight(weight_t weight);
    double parseProfileTime(timestamp_t time);
    double parseExitValue(uint32_t val);

    flow_t writeProfileFlow(double flow);
    pressure_t writeProfilePressure(double pressure);
    percent_t writeProfilePercent(double percent);
    temperature_t writeProfileTemperature(double temperature);
    weight_t writeProfileWeight(double weight);
    timestamp_t writeProfileTime(double time);
    uint32_t writeExitValue(double exit);
}

#endif //MICROPROFILEENGINE_TYPES_HPP
