include(FetchContent)
FetchContent_Declare(ArduinoJson
        GIT_REPOSITORY https://github.com/bblanchon/ArduinoJson.git
        GIT_TAG        v7.2.1
)
FetchContent_MakeAvailable(ArduinoJson)

#add_executable(exit_trigger ExitTrigger.cpp)
#target_link_libraries(exit_trigger ArduinoJson)
#add_executable(profile_definition ProfileDefinition.cpp)
#target_link_libraries(profile_definition ArduinoJson)
#add_executable(profile_generator ProfileGenerator.cpp)
#target_link_libraries(profile_generator ArduinoJson)
#add_executable(sampler Sampler.cpp)
#target_link_libraries(sampler ArduinoJson)
#add_executable(simplified_profile_engine SimplifiedProfileEngine.cpp)
#target_link_libraries(simplified_profile_engine ArduinoJson)

# Gather all source files in this directory
file(GLOB SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
# Create a library from all .cpp files in src/
add_library(MicroProfileEngine STATIC ${SRC_FILES})

# Link any required libraries (like ArduinoJson)

target_link_libraries(MicroProfileEngine ArduinoJson)
