//
// Created by lohse on 11/28/24.
//

#ifndef MICROPROFILEENGINE_PROFILE_HPP
#define MICROPROFILEENGINE_PROFILE_HPP

#include "unordered_map"

#include "types.hpp"
#include "Stage.hpp"

namespace profile {
    class Profile {
        temperature_t startingTemp;
        weight_t targetWeight;
        bool waiting_after_heating, autoPurge;
        std::unordered_map<uint8_t, Stage> stages;
        std::unordered_map<uint8_t, StageLog> stageLogs;

    public:
        temperature_t getStartingTemp() const;
        weight_t getTargetWeight() const;
        bool shouldWaitAfterHeating() const;
        bool shouldAutoPurge() const;
        const std::unordered_map<uint8_t, const Stage>& getStages() const;
        const std::unordered_map<uint8_t, const StageLog>& getStageLogs() const;
        std::unordered_map<uint8_t, StageLog>& getStageLogs();
    };


}


#endif //MICROPROFILEENGINE_PROFILE_HPP
