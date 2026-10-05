#include "matching/MatchingEngine.h"

#include <algorithm>
#include <cctype>

namespace orbit {

namespace {

std::string normalize(std::string value) {
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

    if (job.skills.empty()) {
        return result;
    }

    for (const auto& job_skill : job.skills) {
        const auto normalized_job_skill = normalize(job_skill);

        bool found = false;

        for (const auto& candidate_skill : candidate.skills) {
            if (normalize(candidate_skill) == normalized_job_skill) {
                found = true;
                break;
            }
        }

        if (found) {
            result.matched_skills.push_back(job_skill);
        } else {
            result.missing_skills.push_back(job_skill);
        }
    }

    result.score =
        static_cast<double>(result.matched_skills.size()) /
        static_cast<double>(job.skills.size());

    if (job.minimum_years_experience.has_value() &&
        candidate.years_of_experience.has_value()) {

        result.experience_match =
            candidate.years_of_experience.value() >=
            job.minimum_years_experience.value();
    } else {
        result.experience_match = true;
    }

    return result;
}

} // namespace orbit