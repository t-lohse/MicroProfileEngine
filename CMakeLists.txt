cmake_minimum_required(VERSION 3.15)
project(MicroProfileEngine CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CPPCHECK_EXECUTABLE cppcheck)



#set(CMAKE_CXX_CPPCHECK "/usr/bin/cppcheck")
# Profiles
# Check for the compiler being used
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "Using Clang: Enabling Clang-specific flags")
    # Only apply analyzer flags for clang-tidy, not the compiler
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall")
    #set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-header-filter=^(?!.*_deps).*")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xclang -clear-ast-before-backend -fplugin=./lib/libAnalysisPlugin.so -fplugin-arg-ptr_analysis-smart-ptr-factory=make_unique")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Using GCC: Enabling GCC-specific flags")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fanalyzer")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=stdlibc++")
    #target_compile_options(main PRIVATE -Xclang -clear-ast-before-backend -fplugin=./lib/libAnalysisPlugin.so -fplugin-arg-ptr_analysis-smart-ptr-factory=make_unique)
else()
    message(STATUS "Unknown compiler: No specific flags set")
endif()

include(FetchContent)

FetchContent_Declare(ArduinoJson
        GIT_REPOSITORY https://github.com/bblanchon/ArduinoJson.git
        GIT_TAG        v7.2.1
)
FetchContent_MakeAvailable(ArduinoJson)



add_executable(main main.cpp ExitTrigger.cpp ProfileDefinition.cpp ProfileGenerator.cpp Sampler.cpp SimplifiedProfileEngine.cpp)

target_link_libraries(main ArduinoJson m)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (NOT DEFINED SafetyAnalysisPlugin)
        message(FATAL_ERROR "No safetyanalysisplugin provided. Use `-DSafetyAnalysisPlugin`")
    endif()
    #cmake -DCMAKE_CXX_COMPILER=clang -DSafetyAnalysisPlugin=/path/to/plugin
    message(STATUS "CLANG ADD TARGET COMPILE OPTIONS")
    message(STATUS "Using safety analysis plugin at ${SafetyAnalysisPlugin}")
    target_compile_options(main PRIVATE -stdlib=libstdc++ -Xclang -clear-ast-before-backend
                           -fplugin=${SafetyAnalysisPlugin}

                           -fplugin-arg-safety_analysis-smart-ptr-factory=make_unique
                           "-fplugin-arg-safety_analysis-ignore-namespaces=gsl\;ArduinoJson"
    )
    target_link_options(main PRIVATE
        -stdlib=libstdc++
        -lstdc++
    )

endif()

