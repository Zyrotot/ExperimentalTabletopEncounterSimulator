cmake_minimum_required(VERSION 3.16)

project(ExperimentalTabletopEncounterSimulator
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(nlohmann_json REQUIRED)

add_executable(ETTES
    src/main.cpp
)

target_compile_options(ETTES PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
)

target_link_libraries(ETTES PRIVATE nlohmann_json::nlohmann_json)

enable_testing()
add_subdirectory(tests)