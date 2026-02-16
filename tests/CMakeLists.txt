include(GoogleTest)

file(GLOB_RECURSE TEST_SOURCES
    CONFIGURE_DEPENDS
    *.cpp
)

add_executable(ETTES_Tests ${TEST_SOURCES})

target_include_directories(ETTES_Tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(ETTES_Tests
    PRIVATE
    ettes_core
    gmock_main
)

gtest_discover_tests(ETTES_Tests)
