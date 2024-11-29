//
// Created by lohse on 11/28/24.
//
#include "sensor.hpp"
#include "iostream"

// DummySensorState
double DummySensorState::pistonPosition() const { return *_pistonPosition; }
double DummySensorState::pistonSpeed() const { return _pistonSpeed; }
double DummySensorState::waterTemperature() const { return _waterTemperature; }
double DummySensorState::cylinderTemperature() const { return _cylinderTemperature; }
double DummySensorState::externalTemperature1() const { return _externalTemperature1; }
double DummySensorState::externalTemperature2() const { return _externalTemperature2; }
double DummySensorState::tubeTemperature() const { return _tubeTemperature; }
double DummySensorState::plungerTemperature() const { return _plungerTemperature; }
double DummySensorState::waterFlow() const { return _waterFlow; }
double DummySensorState::waterPressure() const { return _waterPressure; }
double DummySensorState::predictiveTemperature() const { return _predictiveTemperature; }
double DummySensorState::weight() const { return _weight; }
double DummySensorState::temperatureUp() const { return _temperatureUp; }
double DummySensorState::temperatureMiddleUp() const { return _temperatureMiddleUp; }
double DummySensorState::temperatureMiddleDown() const { return _temperatureMiddleDown; }
double DummySensorState::temperatureDown() const { return _temperatureDown; }
double DummySensorState::outputPosition() const { return _outputPosition; }
double DummySensorState::motorEncoder() const { return _motorEncoder; }
double DummySensorState::stableTemperature() const { return _stableTemperature; }
bool DummySensorState::hasWater() const { return _hasWater; }

// Driver
namespace hardware_connection
{
    bool heatingFinished() { return true; }
    bool hasReachedFinalWeight() { return false; }
    void setTargetWeight(profile::weight_t setPoint)
    {
        std::cout << "Setting target weight to " << setPoint << std::endl;
    }
    void setTargetTemperature(profile::temperature_t setPoint)
    {
        std::cout << "Setting target temperature to " << setPoint << std::endl;
    }
    void setTargetPressure(profile::pressure_t setPoint)
    {
        std::cout << "Setting target pressure to " << setPoint << std::endl;
    }
    void setPressureLimit(profile::pressure_t setPoint)
    {
        std::cout << "Setting pressure limit to " << setPoint << std::endl;
    }
    void setTargetFlow(profile::flow_t setPoint) { std::cout << "Setting target flow to " << setPoint << std::endl; }
    void setFlowLimit(profile::pressure_t setPoint) { std::cout << "Setting flow limit to " << setPoint << std::endl; }
    void setTargetPower(double setPoint) { std::cout << "Setting target power to " << setPoint << std::endl; }
    void setTargetPistonPosition(double setPoint)
    {
        std::cout << "Setting target piston position to " << setPoint << std::endl;
    }
}  // namespace hardware_connection
