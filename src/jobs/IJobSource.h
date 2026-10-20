#pragma once

#include "core/Job.h"

#include <string>
#include <vector>

namespace orbit {

class IJobSource {
public:
    virtual ~IJobSource() = default;

    virtual std::string name() const = 0;
    virtual std::vector<Job> fetch_jobs() = 0;
};

} // namespace orbit