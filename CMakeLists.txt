cmake_minimum_required(VERSION 3.20)

project(ETTES VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── Options ──────────────────────────────────────────────────────────────────
option(BUILD_TESTS     "Build unit tests"       OFF)
option(ENABLE_COVERAGE "Enable code coverage"   OFF)

# ── External Dependencies ────────────────────────────────────────────────────
add_subdirectory(external/json)   # Glaze (header-only, provides glaze::glaze)

# ── Core Library ─────────────────────────────────────────────────────────────
# Auto-discover sources so new .cpp files are picked up automatically.
file(GLOB_RECURSE ETTES_CORE_SOURCES CONFIGURE_DEPENDS src/internal/*.cpp)

add_library(ettes_core ${ETTES_CORE_SOURCES})
target_include_directories(ettes_core PUBLIC src)
target_link_libraries(ettes_core PUBLIC glaze::glaze)

# ── Main Executable ─────────────────────────────────────────────────────────
add_executable(ETTES src/main.cpp)
target_link_libraries(ETTES PRIVATE ettes_core)

# ── Compiler Warnings ───────────────────────────────────────────────────────
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(ettes_core PRIVATE -Wall -Wextra -Wpedantic -Wconversion)
endif()

# ── Code Coverage ────────────────────────────────────────────────────────────
if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(ettes_core PUBLIC --coverage -O0 -g)
        target_link_options(ettes_core PUBLIC --coverage)
    endif()
endif()

# ── Tests ────────────────────────────────────────────────────────────────────
if(BUILD_TESTS OR ENABLE_COVERAGE)
    enable_testing()
    add_subdirectory(external/googletest)
    add_subdirectory(tests)
endif()