#include "RequirementExtractor.h"

#include <string>

namespace orbit {

std::vector<Requirement> RequirementExtractor::extract(
    const ParsedJob& job
) const {
    std::vector<Requirement> requirements;

    for (const auto& skill : job.skills) {
        Requirement requirement{
            .type = RequirementType::Skill,
            .priority = RequirementPriority::Required,
            .value = skill,
            .minimum_years = std::nullopt,
            .evidence = "Detected skill: " + skill
        };

        requirements.push_back(requirement);
    }

    if (job.minimum_years_experience.has_value()) {
        Requirement requirement{
            .type = RequirementType::Experience,
            .priority = RequirementPriority::Required,
            .value = "Software Engineering Experience",
            .minimum_years = job.minimum_years_experience,
            .evidence =
                "Minimum experience requirement detected from job description"
        };

        requirements.push_back(requirement);
    }

    return requirements;
}

} // namespace orbit