#include "matching/MatchingEngine.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <unordered_set>

namespace orbit {

namespace {

std::string normalize(std::string value) {
    // Remove leading whitespace.
    const auto first = std::find_if_not(
        value.begin(),
        value.end(),
        [](unsigned char c) {
            return std::isspace(c);
        }
    );

    // Remove trailing whitespace.
    const auto last = std::find_if_not(
        value.rbegin(),
        value.rend(),
        [](unsigned char c) {
            return std::isspace(c);
        }
    ).base();

    if (first >= last) {
        return {};
    }

    value = std::string(first, last);

    // Convert to lowercase.
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );

    return value;
}

} // namespace

MatchResult MatchingEngine::match(
    const Candidate& candidate,
    const Job& job
) const {
    MatchResult result;

    // Normalize candidate skills.
    std::unordered_set<std::string> candidate_skills;

    for (const auto& skill : candidate.skills) {
        const auto normalized = normalize(skill);

        if (!normalized.empty()) {
            candidate_skills.insert(normalized);
        }
    }

    // Process job requirements without duplicates.
    std::unordered_set<std::string> processed_job_skills;

    for (const auto& job_skill : job.skills) {
        const auto normalized = normalize(job_skill);

        if (normalized.empty()) {
            continue;
        }

        // Ignore duplicate job requirements.
        if (!processed_job_skills.insert(normalized).second) {
            continue;
        }

        if (candidate_skills.count(normalized) > 0) {
            result.matched_skills.push_back(job_skill);
        } else {
            result.missing_skills.push_back(job_skill);
        }
    }

    // Calculate skill compatibility score.
    if (!processed_job_skills.empty()) {
        result.score =
            static_cast<double>(result.matched_skills.size()) /
            static_cast<double>(processed_job_skills.size());
    }

    // Evaluate experience compatibility independently.
    if (!job.minimum_years_experience.has_value()) {
        result.experience_status =
            ExperienceMatchStatus::NotRequired;
    } else if (!candidate.years_of_experience.has_value()) {
        result.experience_status =
            ExperienceMatchStatus::Unknown;
    } else if (
        candidate.years_of_experience.value() >=
        job.minimum_years_experience.value()
    ) {
        result.experience_status =
            ExperienceMatchStatus::Met;
    } else {
        result.experience_status =
            ExperienceMatchStatus::NotMet;
    }

    return result;
}

} // namespace orbit