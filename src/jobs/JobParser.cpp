#include "JobParser.h"

#include <algorithm>
#include <cctype>
#include <regex>

namespace orbit {

namespace {

std::string normalize(const std::string& value) {
    std::string result = value;

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

bool contains(
    const std::string& text,
    const std::string& keyword
) {
    return normalize(text).find(normalize(keyword))
        != std::string::npos;
}

} // namespace

ParsedJob JobParser::parse(
    const std::string& job_description
) const {
    ParsedJob result;

    result.description = job_description;

    const std::vector<std::string> known_skills = {
        "C++",
        "Python",
        "Java",
        "Go",
        "Rust",
        "Linux",
        "Docker",
        "Kubernetes",
        "AWS",
        "Azure",
        "GCP",
        "Git",
        "SQL",
        "PostgreSQL",
        "Redis",
        "Kafka"
    };

    for (const auto& skill : known_skills) {
        if (contains(job_description, skill)) {
            result.skills.push_back(skill);
        }
    }

    std::regex experience_pattern(
        R"((\d+)\+?\s*(?:years?|yrs?)\s*(?:of\s*)?(?:experience|exp))",
        std::regex::icase
    );

    std::smatch match;

    if (std::regex_search(
            job_description,
            match,
            experience_pattern)) {

        result.minimum_years_experience =
            std::stoi(match[1].str());
    }

    return result;
}

} // namespace orbit