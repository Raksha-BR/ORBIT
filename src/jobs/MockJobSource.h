#pragma once

#include "jobs/IJobSource.h"

#include <vector>

namespace orbit {

class MockJobSource final : public IJobSource {
public:
    std::string name() const override;
    std::vector<Job> fetch_jobs() override;
};

} // namespace orbit