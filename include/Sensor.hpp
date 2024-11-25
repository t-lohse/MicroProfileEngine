#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <string>

struct SensorState
{
public:
    double piston_position;
    double piston_speed;
    double water_temp;
    double cylinder_temperature;
    double external_temperature_1;
    double external_temperature_2;
    double tube_temperature;
    double plunger_temperature;
    double water_flow;
    double water_pressure;
    double predictive_temperature;
    double weight;
    bool has_water;
    double motor_encoder;       // DEPRECATED
    double stable_temperature;  // DEPRECATED
    double temperature_up;
    double temperature_middle_up;
    double temperature_middle_down;
    double temperature_down;
    double output_position;
};

class Driver
{
public:
    Driver(): sensors() {}

    SensorState get_sensor_data() { return sensors; }

    bool get_button_gesture(std::string source, std::string gesture) { return false; }

    SensorState sensors;
};

#endif