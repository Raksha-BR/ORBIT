#pragma once

#include <optional>
#include <string>
#include <vector>

namespace orbit {

struct JobSearchFilters {
    std::optional<std::string> keyword;
    std::optional<std::string> location;
    std::optional<int> maximum_experience_years;
    std::vector<std::string> required_skills;
};

} // namespace orbit