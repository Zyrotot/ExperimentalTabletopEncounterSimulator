find_package(GTest REQUIRED)
include(GoogleTest)

file(GLOB_RECURSE TEST_SOURCES
    CONFIGURE_DEPENDS
    *.cpp
)

add_executable(ETTES_Tests
    ${TEST_SOURCES}
)

target_link_libraries(ETTES_Tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
)

gtest_discover_tests(ETTES_Tests)
