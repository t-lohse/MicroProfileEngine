//
// Created by lohse on 11/28/24.
//
#ifndef MICROPROFILEENGINE_STAGE_HPP
#define MICROPROFILEENGINE_STAGE_HPP

#include "vector"
#include "optional"
#include "Dynamics.hpp"
#include "ExitTrigger.hpp"
#include "types.hpp"
namespace profile
{
    class Stage
    {
        ControlType controlType;
        Dynamics dynamics;
        std::vector<ExitTrigger> exitTriggers;
        std::vector<Limit> limits;

    public:
        explicit Stage(ControlType controlType, Dynamics&& dynamics, std::vector<ExitTrigger> exitTriggers,
                       std::vector<Limit> limits);
        const Dynamics& getDynamics() const;
        const std::vector<ExitTrigger>& getExitTriggers() const;
        const std::vector<Limit>& getLimits() const;
        const ControlType getControlType() const;
        /* Mutable
        std::vector<ExitTrigger>& getExitTriggers();
        std::vector<Limit>& getLimits();
        ControlType getControlType();
         */

    };

    class StageVariables
    {
        flow_t flow;
        pressure_t pressure;
        percent_t piston_pos;
        std::chrono::time_point<std::chrono::system_clock> timestamp;

    public:
        explicit StageVariables(flow_t flow, pressure_t pressure, percent_t piston_pos,
                                std::chrono::time_point<std::chrono::system_clock> timestamp);
        const std::chrono::time_point<std::chrono::system_clock>& getTimestamp() const;
    };

    class StageLog
    {
        std::optional<StageVariables> entry;
        std::optional<StageVariables> exit;

    public:
        explicit StageLog() = default;
        bool isValid() const;
        std::optional<gsl::not_null<const StageVariables*>> getEntry() const;
        std::optional<gsl::not_null<StageVariables*>> getEntry();
        std::optional<StageVariables> putEntry(StageVariables s);

        std::optional<gsl::not_null<const StageVariables*>> getExit() const;
        std::optional<gsl::not_null<StageVariables*>> getExit();
        std::optional<StageVariables> putExit(StageVariables s);
    };
}  // namespace profile

#endif  // MICROPROFILEENGINE_STAGE_HPP
