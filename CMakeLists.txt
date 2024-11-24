cmake_minimum_required(VERSION 3.15)
project(MicroProfileEngine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
#set(CMAKE_CXX_COMPILER "clang++")
#add_compile_options(-Wlifetime)

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
#add_subdirectory(tests)
