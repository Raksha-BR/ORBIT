#pragma once

#include "core/Job.h"

#include <cstddef>
#include <string>
#include <vector>

namespace orbit {

struct JobSourceResult {
    std::string source_name;
    std::vector<Job> jobs;
    std::size_t fetched_count{0};
    std::size_t rejected_count{0};
    std::string error;
};

} // namespace orbit