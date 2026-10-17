
#include "InMemoryJobRepository.h"

namespace orbit {

void InMemoryJobRepository::save(const Job& job) {
    jobs_[job.id] = job;
}

std::optional<Job> InMemoryJobRepository::find(JobId id) const {
    const auto it = jobs_.find(id);

    if (it == jobs_.end()) {
        return std::nullopt;
    }

    return it->second;
}

std::vector<Job> InMemoryJobRepository::find_all() const {
    std::vector<Job> result;
    result.reserve(jobs_.size());

    for (const auto& [id, job] : jobs_) {
        result.push_back(job);
    }

    return result;
}

} // namespace orbit
