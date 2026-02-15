include(GoogleTest)

file(GLOB_RECURSE TEST_SOURCES
    CONFIGURE_DEPENDS
    *.cpp
)

add_executable(ETTES_Tests
    ${TEST_SOURCES}
)

target_sources(ETTES_Tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/internal/dice_rolls/roller.cpp
    ${CMAKE_SOURCE_DIR}/src/internal/dice_rolls/random_engine.cpp
)

target_include_directories(ETTES_Tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external/googletest/googlemock/include
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(ETTES_Tests
    PRIVATE
    gmock_main
)

gtest_discover_tests(ETTES_Tests)
