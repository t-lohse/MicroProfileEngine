//
// Created by lohse on 11/28/24.
//
#include "Profile.hpp"

namespace profile
{

    temperature_t Profile::getStartingTemp() const { return startingTemp; }
    weight_t Profile::getTargetWeight() const { return targetWeight; }
    bool Profile::shouldWaitAfterHeating() const { return waiting_after_heating; }
    bool Profile::shouldAutoPurge() const { return autoPurge; }
    const std::vector<Stage>& Profile::getStages() const { return stages; }

    [[maybe_unused]] const std::vector<StageLog>& Profile::getStageLogs() const { return stageLogs; }
    std::vector<StageLog>& Profile::getStageLogs() { return stageLogs; }
}  // namespace profile