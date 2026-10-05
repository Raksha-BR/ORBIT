#include "RequirementNormalizer.h"

#include <algorithm>
#include <cctype>

namespace orbit {

std::string RequirementNormalizer::normalize_skill(
    const std::string& skill
) const {
    std::string result = skill;

    std::transform(
        result.begin(),
        result.end(),
        result.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );

    return result;
}

} // namespace orbit