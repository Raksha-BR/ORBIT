
#pragma once

#include "IJobRepository.h"

#include <unordered_map>

namespace orbit {

class InMemoryJobRepository : public IJobRepository {
public:
    void save(const Job& job) override;

    std::optional<Job> find(JobId id) const override;

    std::vector<Job> find_all() const override;

private:
    std::unordered_map<JobId, Job> jobs_;
};

} // namespace orbit
