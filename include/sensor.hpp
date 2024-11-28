//
// Created by lohse on 11/28/24.
//

#ifndef MICROPROFILEENGINE_SENSOR_HPP
#define MICROPROFILEENGINE_SENSOR_HPP

#include "concepts"
#include "string"
#include "gsl/gsl"

#include "types.hpp"


template <typename T>
concept SensorState = requires(T v) {
    {v.pistonPosition()} -> std::same_as<double>;
    {v.pistonSpeed()} -> std::same_as<double>;
    {v.waterTemperature()} -> std::same_as<double>;
    {v.cylinderTemperature()} -> std::same_as<double>;
    {v.externalTemperature1()} -> std::same_as<double>;
    {v.externalTemperature2()} -> std::same_as<double>;
    {v.tubeTemperature()} -> std::same_as<double>;
    {v.plungerTemperature()} -> std::same_as<double>;
    {v.waterFlow()} -> std::same_as<double>;
    {v.waterPressure()} -> std::same_as<double>;
    {v.predictiveTemperature()} -> std::same_as<double>;
    {v.weight()} -> std::same_as<double>;
    {v.temperatureUp()} -> std::same_as<double>;
    {v.temperatureMiddleUp()} -> std::same_as<double>;
    {v.temperatureMiddleDown()} -> std::same_as<double>;
    {v.temperatureDown()} -> std::same_as<double>;
    {v.outputPosition()} -> std::same_as<double>;
    {v.motorEncoder()} -> std::same_as<double>;
    {v.stableTemperature()} -> std::same_as<double>;
    {v.hasWater()} -> std::same_as<bool>;
};


template <SensorState T>
class Driver {
    T sensors;

public:
    const T& getSensorState( std::string _source, std::string _gesture) const;
    bool getButtonGesture() const;
    bool heatingFinished() const;
    bool hasReachedFinalWeight()const;
    void setTargetWeight(profile::weight_t setPoint);
    void setTargetTemperature(profile::temperature_t setPoint);
    void setTargetPressure(profile::pressure_t setPoint);
    void setPressureLimit(profile::pressure_t setPoint);
    void setTargetFlow(profile::flow_t setPoint);
    void setFlowLimit(profile::pressure_t setPoint);
    void setTargetPower(double setPoint);
    void setTargetPistonPosition(double setPoint);
};

struct DummySensorState {
    gsl::not_null<std::unique_ptr<double>> _pistonPosition;
    double _pistonSpeed;
    double _waterTemperature;
    double _cylinderTemperature;
    double _externalTemperature1;
    double _externalTemperature2;
    double _tubeTemperature;
    double _plungerTemperature;
    double _waterFlow;
    double _waterPressure;
    double _predictiveTemperature;
    double _weight;
    double _temperatureUp;
    double _temperatureMiddleUp;
    double _temperatureMiddleDown;
    double _temperatureDown;
    double _outputPosition;
    double _motorEncoder;
    double _stableTemperature;
    bool _hasWater;

    double pistonPosition() const;
    double pistonSpeed() const;
    double waterTemperature() const;
    double cylinderTemperature() const;
    double externalTemperature1() const;
    double externalTemperature2() const;
    double tubeTemperature() const;
    double plungerTemperature() const;
    double waterFlow() const;
    double waterPressure() const;
    double predictiveTemperature() const;
    double weight() const;
    double temperatureUp() const;
    double temperatureMiddleUp() const;
    double temperatureMiddleDown() const;
    double temperatureDown() const;
    double outputPosition() const;
    double motorEncoder() const;
    double stableTemperature() const;
    bool hasWater()const;
};


#endif //MICROPROFILEENGINE_SENSOR_HPP