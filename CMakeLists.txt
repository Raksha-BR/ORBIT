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

find_package(PostgreSQL REQUIRED)

add_library(orbit_core
    src/core/ApplicationStateMachine.cpp
    src/matching/MatchingEngine.cpp
    src/jobs/JobParser.cpp
    src/jobs/requirements/RequirementExtractor.cpp
    src/jobs/requirements/RequirementNormalizer.cpp
    src/candidate/CandidateFactStore.cpp
    src/applications/AnswerEngine.cpp
    src/storage/InMemoryCandidateFactRepository.cpp
    src/storage/postgres/PostgresConnection.cpp
    src/storage/postgres/PostgresSchema.cpp
    src/storage/postgres/PostgresCandidateFactRepository.cpp
    src/storage/postgres/PostgresApplicationRepository.cpp
)

target_include_directories(orbit_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(orbit_core
    PUBLIC
        PostgreSQL::PostgreSQL
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
# Job Intelligence Tests
# ============================================================

add_executable(orbit_job_tests
    tests/jobs/JobParserTest.cpp
)

target_link_libraries(orbit_job_tests
    PRIVATE
        orbit_core
        GTest::gtest_main
)

# ============================================================
# Requirement Intelligence Tests
# ============================================================

add_executable(orbit_requirement_tests
    tests/jobs/requirements/RequirementExtractorTest.cpp
)

target_link_libraries(orbit_requirement_tests
    PRIVATE
        orbit_core
        GTest::gtest_main
)

# ============================================================
# Candidate Intelligence Tests
# ============================================================

add_executable(orbit_candidate_tests
    tests/candidate/CandidateFactStoreTest.cpp
)

target_link_libraries(orbit_candidate_tests
    PRIVATE
        orbit_core
        GTest::gtest_main
)

# ============================================================
# Application Answer Engine Tests
# ============================================================

add_executable(orbit_answer_tests
    tests/applications/AnswerEngineTest.cpp
)

target_link_libraries(orbit_answer_tests
    PRIVATE
        orbit_core
        GTest::gtest_main
)

# ============================================================
# Storage Tests
# ============================================================

add_executable(orbit_storage_tests
    tests/storage/InMemoryCandidateFactRepositoryTest.cpp
)

target_link_libraries(orbit_storage_tests
    PRIVATE
        orbit_core
        GTest::gtest_main
)

add_executable(orbit_postgres_tests
    tests/storage/PostgresConnectionTest.cpp
    tests/storage/PostgresCandidateFactRepositoryTest.cpp
    tests/storage/PostgresApplicationRepositoryTest.cpp
)

target_link_libraries(orbit_postgres_tests
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
gtest_discover_tests(orbit_job_tests)
gtest_discover_tests(orbit_requirement_tests)
gtest_discover_tests(orbit_candidate_tests)
gtest_discover_tests(orbit_answer_tests)
gtest_discover_tests(orbit_storage_tests)
gtest_discover_tests(orbit_postgres_tests)
