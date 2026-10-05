#pragma once

#include <optional>
#include <string>
#include <vector>

namespace orbit {

struct ParsedJob {
    std::string title;
    std::string company;
    std::string location;

    std::optional<int> minimum_years_experience;

    std::vector<std::string> skills;

    std::string description;
};

} // namespace orbit