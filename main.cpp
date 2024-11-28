
//#include "ProfileDefinition.hpp"
//#include "SimplifiedProfileEngine.hpp"
//#include "ProfileGenerator.hpp"
#include "Dynamics.hpp"
#include "Profile.hpp"
#include "sensor.hpp"

#include <chrono>
#include <thread>
#include <algorithm>
#include <gsl/gsl>
#include "iostream"

const char* profileJson = R"JSON({
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


int main(int argc, char** argv)
{
/*
    ProfileGenerator generator(profileJson);
    Profile maxProfile = generator.profile;

    Driver driver;
    SimplifiedProfileEngine engine(&maxProfile, &driver);
    printf("After creating the engine is in state: %d\n", (short)engine.state);

    try
    {
        engine.step();
        printf("After one step without starting the engine is in state: %d\n", (short)engine.state);
        printf("Starting engine\n");
        engine.start();
        printf("The engine is in state: %d\n",(short) engine.state);
        while (engine.state != ProfileState::DONE) {
            engine.step();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            // We fake the piston moving 1% each step to show the piston position samping capabilities
            if (engine.state == ProfileState::BREWING)
                driver.sensors.piston_position = std::min<double>(driver.sensors.piston_position + 1, 100.0);
        }
        printf("Profile execution finished.\n");
        printf("Profile allocated 0x%02lX bytes(%ld kB) of ram for all %d stages combined\n", generator.memoryUsed, generator.memoryUsed / 1024, maxProfile.stages_len);
    }
    catch (const NoStagesInProfileException *&e)
    {
        printf("No Stages in profile!!!");
    }
    */
}