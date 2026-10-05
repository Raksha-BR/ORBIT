#include <gtest/gtest.h>

#include "jobs/JobParser.h"
#include "jobs/requirements/RequirementExtractor.h"

namespace orbit {

TEST(RequirementExtractorTest, ExtractsSkillRequirements) {
    ParsedJob job;

    job.skills = {
        "C++",
        "Python",
        "Docker"
    };

    RequirementExtractor extractor;

    const auto requirements = extractor.extract(job);

    ASSERT_EQ(requirements.size(), 3);

    EXPECT_EQ(
        requirements[0].type,
        RequirementType::Skill
    );

    EXPECT_EQ(
        requirements[0].priority,
        RequirementPriority::Required
    );

    EXPECT_EQ(
        requirements[0].value,
        "C++"
    );
}

TEST(RequirementExtractorTest, ExtractsExperienceRequirement) {
    ParsedJob job;

    job.minimum_years_experience = 3;

    RequirementExtractor extractor;

    const auto requirements = extractor.extract(job);

    ASSERT_EQ(requirements.size(), 1);

    EXPECT_EQ(
        requirements[0].type,
        RequirementType::Experience
    );

    ASSERT_TRUE(
        requirements[0].minimum_years.has_value()
    );

    EXPECT_EQ(
        requirements[0].minimum_years.value(),
        3
    );
}

} // namespace orbit