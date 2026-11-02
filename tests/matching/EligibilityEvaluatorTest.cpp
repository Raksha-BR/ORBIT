#include <gtest/gtest.h>

#include "matching/EligibilityEvaluator.h"

namespace orbit {

TEST(EligibilityEvaluatorTest, QualifiesCandidateMeetingRequirements) {
    MatchResult match_result{
        .score = 0.9,
        .experience_status = ExperienceMatchStatus::Met
    };

    EligibilityEvaluator evaluator;

    const auto result = evaluator.evaluate(match_result);

    EXPECT_EQ(result.status, EligibilityStatus::Qualified);
    EXPECT_TRUE(result.is_qualified());
    EXPECT_DOUBLE_EQ(result.skill_score, 0.9);
    EXPECT_FALSE(result.reasons.empty());
}

TEST(EligibilityEvaluatorTest, RejectsCandidateNotMeetingExperience) {
    MatchResult match_result{
        .score = 0.9,
        .experience_status = ExperienceMatchStatus::NotMet
    };

    EligibilityEvaluator evaluator;

    const auto result = evaluator.evaluate(match_result);

    EXPECT_EQ(result.status, EligibilityStatus::NotQualified);
    EXPECT_FALSE(result.is_qualified());
}

TEST(EligibilityEvaluatorTest, RequiresReviewForUnknownExperience) {
    MatchResult match_result{
        .score = 0.9,
        .experience_status = ExperienceMatchStatus::Unknown
    };

    EligibilityEvaluator evaluator;

    const auto result = evaluator.evaluate(match_result);

    EXPECT_EQ(result.status, EligibilityStatus::NeedsReview);
}

TEST(EligibilityEvaluatorTest, RequiresReviewForInsufficientSkills) {
    MatchResult match_result{
        .score = 0.3,
        .experience_status = ExperienceMatchStatus::Met
    };

    EligibilityEvaluator evaluator;

    const auto result = evaluator.evaluate(match_result);

    EXPECT_EQ(result.status, EligibilityStatus::NeedsReview);
}

TEST(EligibilityEvaluatorTest, QualifiesWhenExperienceIsNotRequired) {
    MatchResult match_result{
        .score = 0.9,
        .experience_status = ExperienceMatchStatus::NotRequired
    };

    EligibilityEvaluator evaluator;

    const auto result = evaluator.evaluate(match_result);

    EXPECT_EQ(result.status, EligibilityStatus::Qualified);
}

TEST(EligibilityEvaluatorTest, SupportsCustomSkillThreshold) {
    MatchResult match_result{
        .score = 0.75,
        .experience_status = ExperienceMatchStatus::Met
    };

    EligibilityEvaluator evaluator(0.8);

    const auto result = evaluator.evaluate(match_result);

    EXPECT_EQ(result.status, EligibilityStatus::NeedsReview);
}

TEST(EligibilityEvaluatorTest, QualifiesAtExactSkillThreshold) {
    MatchResult match_result{
        .score = 0.6,
        .experience_status = ExperienceMatchStatus::Met
    };

    EligibilityEvaluator evaluator(0.6);

    const auto result = evaluator.evaluate(match_result);

    EXPECT_EQ(result.status, EligibilityStatus::Qualified);
}

TEST(EligibilityEvaluatorTest, RejectsInvalidThreshold) {
    EXPECT_THROW(
        EligibilityEvaluator evaluator(1.5),
        std::invalid_argument
    );
}

} // namespace orbit