#pragma once

#include "Requirement.h"

#include "../ParsedJob.h"

#include <vector>

namespace orbit {

class RequirementExtractor {
public:
    std::vector<Requirement> extract(
        const ParsedJob& job
    ) const;
};

} // namespace orbit