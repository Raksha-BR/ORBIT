#pragma once

#include <string>
#include <vector>

namespace orbit {

enum class ExperienceMatchStatus {
    Met,
    NotMet,
    Unknown,
    NotRequired
};

struct MatchResult {
    // Skill compatibility score (0.0 to 1.0).
    double score{0.0};

    std::vector<std::string> matched_skills;
    std::vector<std::string> missing_skills;

    // Experience compatibility is tracked separately.
    ExperienceMatchStatus experience_status{
        ExperienceMatchStatus::Unknown
    };

    // Skill-based match threshold.
    bool is_match() const {
        return score >= 0.6;
    }
};

} // namespace orbit