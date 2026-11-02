#include "CandidateProfileLoader.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace orbit {

namespace {

bool is_usable(const CandidateFact& fact) {
    return fact.confidence != FactConfidence::Uncertain;
}

std::optional<int> parse_integer(const std::string& value) {
    try {
        std::size_t processed = 0;
        const int result = std::stoi(value, &processed);

        if (processed != value.size() || result < 0) {
            return std::nullopt;
        }

        return result;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

} // namespace

Candidate CandidateProfileLoader::load(
    const ICandidateFactRepository& repository
) const {
    Candidate candidate;

    for (const auto& fact : repository.find_all()) {
        if (!is_usable(fact)) {
            continue;
        }

        if (fact.key == "profile.name") {
            candidate.name = fact.value;
        } else if (fact.key == "profile.email") {
            candidate.email = fact.value;
        } else if (fact.key == "profile.location") {
            candidate.location = fact.value;
        } else if (fact.key == "skill") {
            if (!fact.value.empty()) {
                candidate.skills.push_back(fact.value);
            }
        } else if (fact.key == "experience.years") {
            candidate.years_of_experience =
                parse_integer(fact.value);
        } else if (fact.key == "preference.role") {
            if (!fact.value.empty()) {
                candidate.preferred_roles.push_back(fact.value);
            }
        } else if (fact.key == "preference.location") {
            if (!fact.value.empty()) {
                candidate.preferred_locations.push_back(fact.value);
            }
        } else if (fact.key == "preference.expected_salary") {
            candidate.expected_salary =
                parse_integer(fact.value);
        }
    }

    return candidate;
}

} // namespace orbit