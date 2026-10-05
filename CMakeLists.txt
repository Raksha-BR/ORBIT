cmake_minimum_required(VERSION 3.20)

project(ORBIT
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

# ============================================================
# ORBIT Core Library
# ============================================================

add_library(orbit_core
    src/core/ApplicationStateMachine.cpp
    src/matching/MatchingEngine.cpp
)

target_include_directories(orbit_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ============================================================
# GoogleTest
# ============================================================

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)

FetchContent_MakeAvailable(googletest)

# ============================================================
# Core Tests
# ============================================================

add_executable(orbit_core_tests
    tests/core/ApplicationStateMachineTest.cpp
)

target_link_libraries(orbit_core_tests
    PRIVATE
        orbit_core
        GTest::gtest_main
)

# ============================================================
# Matching Tests
# ============================================================

add_executable(orbit_matching_tests
    tests/matching/MatchingEngineTest.cpp
)

target_link_libraries(orbit_matching_tests
    PRIVATE
        orbit_core
        GTest::gtest_main
)

# ============================================================
# Test Discovery
# ============================================================

include(GoogleTest)

gtest_discover_tests(orbit_core_tests)
gtest_discover_tests(orbit_matching_tests)
