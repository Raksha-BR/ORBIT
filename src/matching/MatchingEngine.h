#pragma once

#include "../core/Candidate.h"
#include "core/Job.h"
#include "matching/MatchResult.h"

namespace orbit {

class MatchingEngine {
public:
    MatchResult match(
        const Candidate& candidate,
        const Job& job
    ) const;
};

} // namespace orbit