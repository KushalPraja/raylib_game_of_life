cmake_minimum_required(VERSION 4.1)
project(gol)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib
        GIT_TAG master # Use 'master' for the latest, or a specific commit hash for stability
)

# The below makes raylib available as a target

FetchContent_MakeAvailable(raylib)

add_executable(gol main.cpp)

target_link_libraries(gol PRIVATE raylib)
