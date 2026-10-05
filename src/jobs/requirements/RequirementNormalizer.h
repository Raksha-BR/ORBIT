#pragma once

#include <string>

namespace orbit {

class RequirementNormalizer {
public:
    std::string normalize_skill(
        const std::string& skill
    ) const;
};

} // namespace orbit