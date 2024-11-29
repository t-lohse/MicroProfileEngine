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
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "Using Clang: Enabling Clang-specific flags")
    # Only apply analyzer flags for clang-tidy, not the compiler
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall")
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-header-filter=^(?!.*_deps).*")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Using GCC: Enabling GCC-specific flags")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fanalyzer")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=stdlibc++")
else()
    message(STATUS "Unknown compiler: No specific flags set")
endif()

include(FetchContent)

FetchContent_Declare(GSL
        GIT_REPOSITORY "https://github.com/microsoft/GSL"
        GIT_TAG "v4.1.0"
        GIT_SHALLOW ON
)

FetchContent_MakeAvailable(GSL)

include_directories(${GSL_SOURCE_DIR}/include)

include(sanitizers.cmake)
#include(doctest.cmake)
#include(benchmark.cmake)

include(CheckIncludeFileCXX)

#find_package(Threads REQUIRED)

include_directories(include)


enable_testing()

add_subdirectory(src)

add_executable(main main.cpp)
target_link_libraries(main MicroProfileEngine Microsoft.GSL::GSL)

set(CPPCHECK_EXCLUDE_DIRS ${CMAKE_SOURCE_DIR}/build;${CMAKE_SOURCE_DIR}/cmake-build-debug;${CMAKE_SOURCE_DIR}/cmake-build-release)
set(CPPCHECK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/main.cpp ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include)
# Run Cppcheck before compilation
if(CPPCHECK_EXECUTABLE)
    message(STATUS "Cppcheck found: ${CPPCHECK_EXECUTABLE}")

    # Add Cppcheck target to analyze all .cpp and .h files
    add_custom_target(
            cppcheck
            COMMAND ${CPPCHECK_EXECUTABLE} --inconclusive
                --enable=all
                --disable=missingInclude
                --disable=unusedFunction
                --error-exitcode=1
                ${CPPCHECK_INCLUDE_DIRS}
                main.cpp
            COMMENT "Running Cppcheck static analysis"
    )

    # Make the main build depend on Cppcheck so it's run before compilation
    add_dependencies(main cppcheck)
else()
    message(WARNING "Cppcheck not found, skipping static analysis.")
endif()
#add_subdirectory(tests)

file(GLOB_RECURSE PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
set(COMBINED_FILE "${CMAKE_BINARY_DIR}/combined.cpp")
add_custom_command(
        OUTPUT ${COMBINED_FILE}
        COMMAND ${CMAKE_CXX_COMPILER} -E
        #-nostdinc
        -nodefaultlibs
        #-stdlib=libc++
        -I${CMAKE_SOURCE_DIR}/include
        -I/usr/lib/clang/18/include
        -I/usr/include/c++/14.2.1
        -I${GSL_SOURCE_DIR}/include
        -I${ArduinoJson_SOURCE_DIR}/src
        ../main.cpp
        -o ${COMBINED_FILE}
        DEPENDS ${PROJECT_SOURCES}
        COMMENT "Generating a combined source file: ${COMBINED_FILE}"
)
message("${ArduinoJson_SOURCE_DIR}")
message("${GSL_SOURCE_DIR}")
add_custom_target(GenerateCombinedSources ALL
        DEPENDS ${COMBINED_FILE}
)
