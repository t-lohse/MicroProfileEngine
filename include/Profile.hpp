//
// Created by lohse on 11/28/24.
//

#ifndef MICROPROFILEENGINE_PROFILE_HPP
#define MICROPROFILEENGINE_PROFILE_HPP

#include "unordered_map"

#include "types.hpp"
#include "Stage.hpp"

namespace profile
{
    static std::expected<std::unordered_map<uint8_t, Stage>, ProfileError> fromJsonStages(ArduinoJson::JsonArray&);

    class Profile
    {
        temperature_t startingTemp;
        weight_t targetWeight;
        bool waiting_after_heating, autoPurge;
        std::unordered_map<uint8_t, Stage> stages;
        std::unordered_map<uint8_t, StageLog> stageLogs;

        explicit Profile(temperature_t startingTemp, weight_t targetWeight, bool waiting_after_heating, bool autoPurge,
                         std::unordered_map<uint8_t, Stage>&& stages,
                         std::unordered_map<uint8_t, StageLog>&& stageLogs):
            startingTemp(startingTemp), targetWeight(targetWeight), waiting_after_heating(waiting_after_heating),
            autoPurge(autoPurge), stages(std::move(stages)), stageLogs(std::move(stageLogs))
        {}

    public:
        temperature_t getStartingTemp() const;
        weight_t getTargetWeight() const;
        bool shouldWaitAfterHeating() const;
        bool shouldAutoPurge() const;
        const std::unordered_map<uint8_t, Stage>& getStages() const;
        const std::unordered_map<uint8_t, StageLog>& getStageLogs() const;
        std::unordered_map<uint8_t, StageLog>& getStageLogs();

        static std::expected<Profile, ProfileError> fromJson(ArduinoJson::JsonDocument& obj)
        {
            auto fj = obj["final_weight"];
            if (!fj)
                return std::unexpected(ProfileError::noName("final_weight"));
            if (!fj.is<double>())
                return std::unexpected(ProfileError::typeError("double"));
            auto finalWeight = fj.as<double>();

            auto t = obj["temperature"];
            if (!t)
                return std::unexpected(ProfileError::noName("temperature"));
            if (!t.is<double>())
                return std::unexpected(ProfileError::typeError("double"));
            auto temp = t.as<double>();

            JsonArray stagesJson = obj["stages"];
            auto stag = fromJsonStages(stagesJson);
            if (!stag)
                return std::unexpected(stag.error());
            auto stages = std::move(stag).value();
            std::unordered_map<uint8_t, StageLog> logs{stages.size()};
            for (const auto& v : stages) {
                logs.insert({v.first, StageLog()});
            }

            bool wait = obj["wait_after_heating"] | false;
            bool autoPurge = obj["auto_purge"] | false;

            return Profile(temp, finalWeight, wait, autoPurge, std::move(stages), std::move(logs));
        }
    };

    static std::expected<std::unordered_map<uint8_t, Stage>, ProfileError> fromJsonStages(ArduinoJson::JsonArray& vals)
    {
        if (!vals)
            return std::unexpected(ProfileError::noName("stages"));
        if (vals.size() <= 0)
            return std::unexpected(ProfileError::parseError("No stages, must have at least one"));
        std::unordered_map<std::string, uint8_t> names{};
        names.reserve(vals.size());
        uint8_t i = 1;
        for (const JsonObject& v : vals) {
            auto n = v["name"];
            if (!n)
                return std::unexpected(ProfileError::noName("name"));
            if (!n.is<std::string>())
                return std::unexpected(ProfileError::typeError("string"));
            auto name = n.as<std::string>();

            names.insert({n, i++});
        }
        std::unordered_map<uint8_t, Stage> out{names.size()};
        for (const JsonObject& v : vals) {
            auto name = v["name"].as<std::string>();
            auto ct = ControlType::fromJson(v);
            if (!ct)
                return std::unexpected(ct.error());
            auto ctrl = ct.value();

            auto d = Dynamics::fromJson(v["dynamics"]);
            if (!d)
                return std::unexpected(d.error());
            auto dyn = std::move(d).value();

            JsonArray ea = v["exit_triggers"];
            auto e = ExitTrigger::fromJson(std::move(ea), names);
            if (!e)
                return std::unexpected(e.error());
            auto ext = e.value();

            auto le = v["limits"];
            auto limits = Limit::fromJson(le).value_or(std::vector<Limit>{});

            auto k = names[name];
            out.insert({k, Stage(ctrl, std::move(dyn), ext, limits)});
        }
        return out;
    }

}  // namespace profile

#endif  // MICROPROFILEENGINE_PROFILE_HPP
