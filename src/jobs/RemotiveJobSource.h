#pragma once

#include "jobs/IJobSource.h"

#include <string>
#include <vector>

namespace orbit {

class RemotiveJobSource final : public IJobSource {
public:
    std::string name() const override;
    std::vector<Job> fetch_jobs() override;
};

} // namespace orbit