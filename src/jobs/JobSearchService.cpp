
#include "jobs/JobSearchService.h"

#include "jobs/JobDeduplicator.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <utility>
#include <vector>

namespace orbit {
namespace {

std::string normalize(std::string value) {
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

bool is_blank(const std::string& value) {
    return std::all_of(
        value.begin(),
        value.end(),
        [](unsigned char c) {
            return std::isspace(c);
        }
    );
}

bool contains_keyword(const Job& job, const std::string& keyword) {
    if (normalize(job.title).find(keyword) != std::string::npos ||
        normalize(job.company).find(keyword) != std::string::npos ||
        normalize(job.location).find(keyword) != std::string::npos ||
        normalize(job.description).find(keyword) != std::string::npos) {
        return true;
    }

    for (const auto& skill : job.skills) {
        if (normalize(skill).find(keyword) != std::string::npos) {
            return true;
        }
    }

    return false;
}

} // namespace

JobSearchService::JobSearchService(
    IJobRepository& repository,
    JobDeduplicator& deduplicator
)
    : repository_(repository),
      deduplicator_(deduplicator) {}

std::vector<Job> JobSearchService::save_jobs(std::vector<Job> jobs) {
    std::vector<Job> saved;
    const auto existing_jobs = repository_.find_all();

    for (auto& job : jobs) {
        bool duplicate = false;

        for (const auto& existing : existing_jobs) {
            if (deduplicator_.are_duplicates(job, existing)) {
                duplicate = true;
                break;
            }
        }

        for (const auto& previously_saved : saved) {
            if (deduplicator_.are_duplicates(job, previously_saved)) {
                duplicate = true;
                break;
            }
        }

        if (duplicate) {
            continue;
        }

        repository_.save(job);
        saved.push_back(std::move(job));
    }

    return saved;
}

std::vector<Job> JobSearchService::get_all_jobs() const {
    return repository_.find_all();
}

std::vector<Job> JobSearchService::search_by_keyword(
    const std::string& keyword
) const {
    const auto normalized_keyword = normalize(keyword);

    if (is_blank(normalized_keyword)) {
        return {};
    }

    const auto jobs = repository_.find_all();
    std::vector<Job> matches;

    for (const auto& job : jobs) {
        if (contains_keyword(job, normalized_keyword)) {
            matches.push_back(job);
        }
    }

    return matches;
}

} // namespace orbit
