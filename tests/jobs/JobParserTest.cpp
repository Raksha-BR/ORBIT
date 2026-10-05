#include <algorithm>
#include <gtest/gtest.h>

#include "jobs/JobParser.h"

namespace orbit {

TEST(JobParserTest, ExtractsSkills) {
    const std::string description =
        "We are looking for a backend engineer with "
        "strong C++ and Python skills. "
        "Experience with Linux, Docker and Kubernetes.";

    JobParser parser;

    const auto result = parser.parse(description);

    EXPECT_EQ(result.skills.size(), 5);

    EXPECT_NE(
        std::find(
            result.skills.begin(),
            result.skills.end(),
            "C++"
        ),
        result.skills.end()
    );

    EXPECT_NE(
        std::find(
            result.skills.begin(),
            result.skills.end(),
            "Python"
        ),
        result.skills.end()
    );
}

TEST(JobParserTest, ExtractsExperienceRequirement) {
    const std::string description =
        "Software engineer with 3+ years of experience "
        "in C++ development.";

    JobParser parser;

    const auto result = parser.parse(description);

    ASSERT_TRUE(result.minimum_years_experience.has_value());

    EXPECT_EQ(
        result.minimum_years_experience.value(),
        3
    );
}

} // namespace orbit