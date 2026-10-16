
#pragma once

#include "core/Job.h"

namespace orbit {

class JobDeduplicator {
public:
    bool are_duplicates(
        const Job& first,
        const Job& second
    ) const;
};

} // namespace orbit
