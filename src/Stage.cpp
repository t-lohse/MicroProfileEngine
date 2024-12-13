//
// Created by lohse on 11/28/24.
//

#include "Stage.hpp"

namespace profile
{
    // Stage
    Stage::Stage(ControlType controlType, Dynamics&& dynamics, std::vector<ExitTrigger> exitTriggers,
                 std::vector<Limit> limits):
        controlType{controlType}, dynamics{std::move(dynamics)}, exitTriggers{exitTriggers}, limits{limits}
    {}
    const Dynamics& Stage::getDynamics() const { return dynamics; }
    // Const vals
    const std::vector<ExitTrigger>& Stage::getExitTriggers() const { return exitTriggers; }
    const std::vector<Limit>& Stage::getLimits() const { return limits; }
    const ControlType Stage::getControlType() const { return controlType; }

    /* Mutable
    std::vector<ExitTrigger>& Stage::getExitTriggers()  {
        return exitTriggers;
    }
    std::vector<Limit>& Stage::getLimits()  {
        return limits;
    }
    ControlType Stage::getControlType()  {
        return controlType;
    }
     */

    // StageVariables
    StageVariables::StageVariables(flow_t flow, pressure_t pressure, percent_t piston_pos,
                                   std::chrono::time_point<std::chrono::system_clock> timestamp):
        flow{flow}, pressure{pressure}, piston_pos{piston_pos}, timestamp{timestamp}
    {}

    [[maybe_unused]] const std::chrono::time_point<std::chrono::system_clock>& StageVariables::getTimestamp() const
    {
        return timestamp;
    }

    // StageLog
    bool StageLog::isValid() const { return entry.has_value(); }

    [[maybe_unused]] std::optional<gsl::not_null<const StageVariables*>> StageLog::getEntry() const
    {
        return entry.transform([](auto& v) { return gsl::not_null{&v}; });
    }

    [[maybe_unused]] std::optional<gsl::not_null<StageVariables*>> StageLog::getEntry()
    {
        return entry.transform([](auto& v) { return gsl::not_null{&v}; });
    }
    std::optional<StageVariables> StageLog::putEntry(StageVariables s)
    {
        auto old = entry;
        entry = s;
        return old;
    }

    [[maybe_unused]] std::optional<gsl::not_null<const StageVariables*>> StageLog::getExit() const
    {
        return exit.transform([](auto& v) { return gsl::not_null{&v}; });
    }

    std::optional<gsl::not_null<StageVariables*>> StageLog::getExit()
    {
        return exit.transform([](auto& v) { return gsl::not_null{&v}; });
    }
    std::optional<StageVariables> StageLog::putExit(StageVariables s)
    {
        auto old = exit;
        exit = s;
        return old;
    }
}  // namespace profile