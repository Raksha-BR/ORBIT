#include <gtest/gtest.h>

#include "matching/MatchingEngine.h"

namespace orbit {

TEST(MatchingEngineTest, CalculatesSkillMatch) {
    Candidate candidate{
        .id = 1,
        .name = "Raksha",
        .email = "test@example.com",
        .location = "Bengaluru",
        .skills = {"C++", "Python", "Git"},
        .preferred_roles = {"SDE"},
        .preferred_locations = {"Bengaluru"},
        .years_of_experience = 1,
        .expected_salary = 1500000
    };

    Job job{
        .id = 1,
        .title = "Software Development Engineer",
        .company = "Example",
        .location = "Bengaluru",
        .salary = std::nullopt,
        .minimum_years_experience = 1,
        .skills = {"C++", "Python", "Docker"},
        .description = "Backend software engineering",
        .application_url = "https://example.com/job"
    };

    MatchingEngine engine;

    const auto result = engine.match(candidate, job);

    EXPECT_DOUBLE_EQ(result.score, 2.0 / 3.0);
    EXPECT_EQ(result.matched_skills.size(), 2);
    EXPECT_EQ(result.missing_skills.size(), 1);
    EXPECT_EQ(result.missing_skills[0], "Docker");


}

TEST(MatchingEngineTest, IgnoresSkillWhitespaceAndCase) {
    Candidate candidate{
        .id = 1,
        .skills = {" C++ ", "PYTHON"}
    };

    Job job{
        .id = 1,
        .skills = {"c++", " Python "}
    };

    MatchingEngine engine;
    const auto result = engine.match(candidate, job);

    EXPECT_DOUBLE_EQ(result.score, 1.0);
    EXPECT_EQ(result.matched_skills.size(), 2);
    EXPECT_TRUE(result.missing_skills.empty());
}


TEST(MatchingEngineTest, IgnoresDuplicateJobRequirements) {
    Candidate candidate{
        .id = 1,
        .skills = {"C++", "Python"}
    };

    Job job{
        .id = 1,
        .skills = {"C++", "Python", "Docker", "Python"}
    };

    MatchingEngine engine;
    const auto result = engine.match(candidate, job);

    EXPECT_DOUBLE_EQ(result.score, 2.0 / 3.0);
    EXPECT_EQ(result.matched_skills.size(), 2);
    EXPECT_EQ(result.missing_skills.size(), 1);
    EXPECT_EQ(result.missing_skills[0], "Docker");
}


} // namespace orbit