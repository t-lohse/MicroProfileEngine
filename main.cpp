#include <thread>
#include <iostream>

#include "Dynamics.hpp"
#include "Profile.hpp"
#include "sensor.hpp"
#include "engine.hpp"

const char* const profileJson = R"JSON({
    "name": "E61 with dropping pressure",
    "id": "4cdc0015-07cd-4738-b198-c7d8742acd2b",
    "author": "Carlos",
    "author_id": "d9123a0a-d3d7-40fd-a548-b81376e43f23",
    "previous_authors": [
        {
            "name": "mimoja",
            "author_id": "d9123a0a-d3d7-40fd-a548-b81376e43f23",
            "profile_id": "0cdf18ca-d72e-4776-8e25-7b3279907dce"
        },
        {
            "name": "Alu",
            "author_id": "ee86a777-fdd6-46d6-8cf7-099a9fb609a1",
            "profile_id": "58036fd5-7d5b-4647-9ab6-2832014bb9be"
        }
    ],
    "temperature": 92.5,
    "final_weight": 80.0,
    "stages": [
        {
            "name": "stage 1",
            "type": "power",
            "dynamics": {
                "points": [
                    [0, 100],
                    [10, 50],
                    [20, 40]
                ],
                "over": "piston_position",
                "interpolation": "linear"
            },
            "exit_triggers": [
                {
                    "type": "time",
                    "value": 3
                },
                {
                    "type": "pressure",
                    "value": 4
                }
            ]
        },
        {
            "name": "stage 2",
            "type": "flow",
            "dynamics": {
                "points": [
                    [0, 8.5],
                    [30, 6.5]
                ],
                "over": "time",
                "interpolation": "linear"
            },
            "exit_triggers": [
                {
                    "type": "time",
                    "value": 2,
                    "relative": true
                }
            ],
            "limits": [
                {
                    "type": "flow",
                    "value": 3
                }
            ]
        }
    ]
})JSON";

int main()
{
    JsonDocument doc;
    deserializeJson(doc, profileJson);

    auto profile = profile::Profile::fromJson(doc).value();

    auto driver = Driver<DummySensorState>();

    auto pistonPos = driver.getSensorState()._pistonPosition;
    auto engineIdle = ProfileEngineIdle(std::move(driver), &profile);
    std::cout << "Starting engine" << std::endl;
    auto engine = std::move(engineIdle).start();
    std::cout << "The engine is in state: " << engine.getState() << std::endl;

    while (engine.getState() != ProfileState::Done) {
        auto newEng = std::move(engine).step();
        bool dip = false;
        switch (newEng) {
            using T = EngineStepResult<DummySensorState>;
            case T::Next:
                engine = std::move(newEng).getNext();
                break;
            case T::Finished:
                dip = true;
                break;
            case T::Error:
                std::cout << "No stages in profile!!! Error: `" << std::move(newEng).getError() << "`" << std::endl;
                return 1;
        }
        if (dip)
            break;

        const long SLEEP_TIME = 50;
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
        std::cout << "The engine is in state: " << engine.getState() << std::endl;

        if (engine.getState() == ProfileState::Brewing) {
            const double PISTON_CAP = 100;
            *pistonPos = std::min<double>(*pistonPos + 1, PISTON_CAP);
            std::cout << "Piston: " << *pistonPos << std::endl;
        }
    }
    std::cout << "Profile execution finished." << std::endl;
}