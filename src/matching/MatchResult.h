#pragma once

#include <string>
#include <vector>

namespace orbit {

struct MatchResult {
    double score{0.0};

    std::vector<std::string> matched_skills;
    std::vector<std::string> missing_skills;

    bool experience_match{false};

    bool is_match() const {
        return score >= 0.6;
    }
};

} // namespace orbit