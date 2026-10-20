#include "jobs/JobValidator.h"

#include <algorithm>
#include <cctype>

namespace orbit {
namespace {

bool blank(const std::string& value) {
    return std::all_of(
        value.begin(),
        value.end(),
        [](unsigned char c) {
            return std::isspace(c) != 0;
        }
    );
}

} // namespace

std::vector<std::string> JobValidator::validate(const Job& job) {
    std::vector<std::string> errors;

    if (blank(job.title)) {
        errors.emplace_back("Job title is required");
    }

    if (blank(job.company)) {
        errors.emplace_back("Company is required");
    }

    if (blank(job.location)) {
        errors.emplace_back("Location is required");
    }

    if (job.minimum_years_experience &&
        *job.minimum_years_experience < 0) {
        errors.emplace_back("Minimum experience cannot be negative");
    }

    return errors;
}

} // namespace orbit