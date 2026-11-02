#pragma once

#include "matching/EligibilityResult.h"
#include "matching/MatchResult.h"

namespace orbit {

class EligibilityEvaluator {
public:
    explicit EligibilityEvaluator(
        double minimum_skill_score = 0.6
    );

    EligibilityResult evaluate(
        const MatchResult& match_result
    ) const;

private:
    double minimum_skill_score_;
};

} // namespace orbit