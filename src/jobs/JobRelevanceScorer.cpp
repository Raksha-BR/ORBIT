
#include "jobs/JobRelevanceScorer.h"

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <vector>

namespace orbit {
namespace {

std::string lowercase(std::string value) {
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );
    return value;
}

bool contains_any(
    const std::string& text,
    const std::vector<std::string>& terms
) {
    return std::any_of(
        terms.begin(),
        terms.end(),
        [&text](const std::string& term) {
            return text.find(term) != std::string::npos;
        }
    );
}

std::optional<double> salary_in_lpa(const std::string& salary) {
    const std::string text = lowercase(salary);

    // Only interpret explicit Indian LPA values.
    // Do not guess conversions from USD or other currencies.
    const auto position = text.find("lpa");
    if (position == std::string::npos) {
        return std::nullopt;
    }

    std::size_t start = position;
    while (start > 0 &&
           (std::isdigit(static_cast<unsigned char>(text[start - 1])) ||
            text[start - 1] == '.')) {
        --start;
    }

    if (start == position) {
        return std::nullopt;
    }

    try {
        return std::stod(text.substr(start, position - start));
    } catch (...) {
        return std::nullopt;
    }
}

void add_reason(
    JobRelevanceScore& result,
    int points,
    const std::string& reason
) {
    result.score += points;
    result.reasons.push_back(
        (points >= 0 ? "+" : "") +
        std::to_string(points) + ": " + reason
    );
}

} // namespace

JobRelevanceScore JobRelevanceScorer::score(const Job& job) const {
    JobRelevanceScore result;
    result.score = 30;

    const std::string title = lowercase(job.title);
    const std::string description = lowercase(job.description);
    const std::string location = lowercase(job.location);

    // Role relevance: prioritize software engineering roles.
    if (contains_any(title, {
            "software engineer",
            "software developer",
            "sde",
            "backend engineer",
            "back-end engineer",
            "backend developer",
            "systems engineer",
            "platform engineer",
            "c++ engineer",
            "python engineer"
        })) {
        add_reason(result, 30, "Strong software engineering role match");
    } else if (contains_any(title, {
                   "engineer",
                   "developer",
                   "programmer"
               })) {
        add_reason(result, 15, "Related engineering or development role");
    } else if (contains_any(title, {
                   "designer",
                   "recruiter",
                   "sales",
                   "account manager",
                   "project manager",
                   "marketing"
               })) {
        add_reason(result, -25, "Title does not match the target SDE role");
    } else {
        add_reason(result, -5, "Software engineering role is not clearly stated");
    }

    // Seniority: avoid automatically prioritizing roles beyond the target level.
    if (contains_any(title, {
            "staff engineer",
            "principal engineer",
            "engineering director",
            "vice president",
            "vp of engineering"
        })) {
        add_reason(result, -20, "Role appears substantially more senior");
    } else if (contains_any(title, {
                   "senior",
                   "sr.",
                   "lead engineer",
                   "tech lead"
               })) {
        add_reason(result, -12, "Senior or lead role may exceed target experience");
    } else if (contains_any(title, {
                   "junior",
                   "entry level",
                   "entry-level",
                   "graduate",
                   "sde i",
                   "sde 1"
               })) {
        add_reason(result, 8, "Entry-level role is compatible with target level");
    }

    // Technologies in the title and description are stronger signals
    // than generic tags returned by some job APIs.
    int technology_points = 0;

    if (contains_any(title + " " + description, {
            "c++", "c/c++", "cpp"
        })) {
        technology_points += 5;
        result.reasons.push_back("+5: C++ appears in the role details");
    }

    if (contains_any(title + " " + description, {
            "python"
        })) {
        technology_points += 5;
        result.reasons.push_back("+5: Python appears in the role details");
    }

    if (contains_any(title + " " + description, {
            "distributed systems",
            "backend",
            "back-end",
            "rest api",
            "microservices",
            "storage systems",
            "linux"
        })) {
        technology_points += 5;
        result.reasons.push_back(
            "+5: Backend, systems, or infrastructure work is mentioned"
        );
    }

    result.score += std::min(technology_points, 15);

    // Skills tags contribute a smaller, capped amount because some
    // sources attach broad lists of skills to unrelated listings.
    int skill_points = 0;

    for (const auto& raw_skill : job.skills) {
        const std::string skill = lowercase(raw_skill);

        if (skill == "c++" || skill == "python" || skill == "c") {
            skill_points += 2;
        } else if (skill == "java" || skill == "linux" ||
                   skill == "sql" || skill == "postgresql") {
            skill_points += 1;
        }
    }

    skill_points = std::min(skill_points, 8);

    if (skill_points > 0) {
        add_reason(
            result,
            skill_points,
            "Relevant technologies appear in the skills list"
        );
    }

    // Experience information: unknown values are not penalized.
    if (job.minimum_years_experience.has_value()) {
        const int years = *job.minimum_years_experience;

        if (years <= 2) {
            add_reason(result, 10, "Experience requirement is up to 2 years");
        } else if (years <= 3) {
            add_reason(result, 5, "Experience requirement is up to 3 years");
        } else if (years >= 5) {
            add_reason(result, -15, "Experience requirement is at least 5 years");
        } else {
            add_reason(result, -5, "Experience requirement may be above target");
        }
    }

    // Location: remote roles are acceptable; Bengaluru is preferred.
    if (contains_any(location, {
            "bengaluru",
            "bangalore"
        })) {
        add_reason(result, 10, "Preferred Bengaluru location");
    } else if (contains_any(location, {
                   "remote",
                   "worldwide",
                   "anywhere"
               })) {
        add_reason(result, 8, "Remote location is acceptable");
    } else if (!location.empty() &&
               !contains_any(location, {"india", "indian"})) {
        add_reason(result, -3, "Location may not match the preferred region");
    }

    // Salary: score only when the listing explicitly states an LPA figure.
    if (job.salary.has_value()) {
        const auto lpa = salary_in_lpa(*job.salary);

        if (lpa.has_value()) {
            if (*lpa >= 15.0) {
                add_reason(result, 10, "Stated salary meets the ₹15 LPA target");
            } else {
                add_reason(result, -10, "Stated salary is below ₹15 LPA");
            }
        }
    }

    result.score = std::clamp(result.score, 0, 100);
    return result;
}

} // namespace orbit
