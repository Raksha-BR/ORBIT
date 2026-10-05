#pragma once

#include "Types.h"

#include <optional>
#include <string>
#include <vector>

namespace orbit {

struct Candidate {
    CandidateId id{};

    std::string name;
    std::string email;
    std::string location;

    std::vector<std::string> skills;
    std::vector<std::string> preferred_roles;
    std::vector<std::string> preferred_locations;

    std::optional<int> years_of_experience;
    std::optional<int> expected_salary;
};

} // namespace orbit
