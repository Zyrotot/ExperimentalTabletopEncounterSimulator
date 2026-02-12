cmake_minimum_required(VERSION 3.16)

project(ExperimentalTabletopEncounterSimulator
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if (ENABLE_COVERAGE)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Coverage enabled")

        add_compile_options(--coverage -O0 -g)
        add_link_options(--coverage)
    else()
        message(WARNING "Coverage requested but not supported by this compiler")
    endif()
endif()

add_subdirectory(external/json)

add_executable(ETTES
    src/main.cpp
)

target_include_directories(ETTES PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_options(ETTES PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
)

target_link_libraries(ETTES PRIVATE nlohmann_json::nlohmann_json)

option(BUILD_TESTS "Build tests" ON)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(external/googletest)
    add_subdirectory(tests)
endif()
