
#pragma once

#include "core/Job.h"

#include <string>
#include <vector>

namespace orbit {

struct JobRelevanceScore {
    int score{0};
    std::vector<std::string> reasons;
};

class JobRelevanceScorer {
public:
    JobRelevanceScore score(const Job& job) const;
};

} // namespace orbit
