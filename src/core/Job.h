#pragma once

#include "Types.h"

#include <optional>
#include <string>
#include <vector>

namespace orbit {

struct Job {
    JobId id{};

    std::string title;
    std::string company;
    std::string location;

    std::optional<std::string> salary;
    std::optional<int> minimum_years_experience;

    std::vector<std::string> skills;

    std::string description;
    std::string application_url;
};

} // namespace orbit
