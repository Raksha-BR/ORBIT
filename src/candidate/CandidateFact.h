#pragma once

#include "Evidence.h"

#include <string>
#include <vector>

namespace orbit {

enum class FactConfidence {
    Verified,
    Derived,
    Uncertain
};

struct CandidateFact {
    std::string key;
    std::string value;

    FactConfidence confidence;

    std::vector<Evidence> evidence;
};

} // namespace orbit