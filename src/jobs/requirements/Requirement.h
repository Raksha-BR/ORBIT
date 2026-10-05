#pragma once

#include <optional>
#include <string>

namespace orbit {

enum class RequirementType {
    Skill,
    Experience,
    Education,
    Location,
    Other
};

enum class RequirementPriority {
    Required,
    Preferred
};

struct Requirement {
    RequirementType type;
    RequirementPriority priority;

    std::string value;

    std::optional<int> minimum_years;

    std::string evidence;
};

} // namespace orbit