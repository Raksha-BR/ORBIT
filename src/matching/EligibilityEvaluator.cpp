#include "matching/EligibilityEvaluator.h"

#include <stdexcept>

namespace orbit {

EligibilityEvaluator::EligibilityEvaluator(
    double minimum_skill_score
)
    : minimum_skill_score_(minimum_skill_score) {

    if (
        minimum_skill_score_ < 0.0 ||
        minimum_skill_score_ > 1.0
    ) {
        throw std::invalid_argument(
            "Minimum skill score must be between 0.0 and 1.0"
        );
    }
}

EligibilityResult EligibilityEvaluator::evaluate(
    const MatchResult& match_result
) const {
    EligibilityResult result;

    result.skill_score = match_result.score;

    // Reject candidates who do not meet mandatory experience.
    if (
        match_result.experience_status ==
        ExperienceMatchStatus::NotMet
    ) {
        result.status = EligibilityStatus::NotQualified;

        result.reasons.push_back(
            "Candidate does not meet the minimum experience requirement."
        );

        return result;
    }

    // Unknown experience requires manual verification.
    if (
        match_result.experience_status ==
        ExperienceMatchStatus::Unknown
    ) {
        result.status = EligibilityStatus::NeedsReview;

        result.reasons.push_back(
            "Candidate experience information is unavailable."
        );

        return result;
    }

    // Evaluate skill compatibility.
    if (match_result.score < minimum_skill_score_) {
        result.status = EligibilityStatus::NeedsReview;

        result.reasons.push_back(
            "Skill compatibility is below the configured threshold."
        );

        return result;
    }

    result.status = EligibilityStatus::Qualified;

    result.reasons.push_back(
        "Candidate meets the configured skill and experience criteria."
    );

    return result;
}

} // namespace orbit