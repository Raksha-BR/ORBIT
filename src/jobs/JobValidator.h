#pragma once

#include "core/Job.h"

#include <string>
#include <vector>

namespace orbit {

class JobValidator {
public:
    static std::vector<std::string> validate(const Job& job);
};

} // namespace orbit