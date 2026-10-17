
#pragma once

#include "../core/Job.h"

#include <optional>
#include <vector>

namespace orbit {

class IJobRepository {
public:
    virtual ~IJobRepository() = default;

    virtual void save(const Job& job) = 0;

    virtual std::optional<Job> find(JobId id) const = 0;

    virtual std::vector<Job> find_all() const = 0;
};

} // namespace orbit
