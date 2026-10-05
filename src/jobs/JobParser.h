#pragma once

#include "ParsedJob.h"

#include <string>

namespace orbit {

class JobParser {
public:
    ParsedJob parse(const std::string& job_description) const;
};

} // namespace orbit