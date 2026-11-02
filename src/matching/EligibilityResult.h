#pragma once

#include <string>
#include <vector>

namespace orbit {

enum class EligibilityStatus {
    Qualified,
    NotQualified,
    NeedsReview
};

struct EligibilityResult {
    EligibilityStatus status{
        EligibilityStatus::NeedsReview
    };

    double skill_score{0.0};

    std::vector<std::string> reasons;

    bool is_qualified() const {
        return status == EligibilityStatus::Qualified;
    }
};

} // namespace orbit