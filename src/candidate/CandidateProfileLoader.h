#pragma once

#include "core/Candidate.h"
#include "storage/IRepository.h"

namespace orbit {

class CandidateProfileLoader {
public:
    Candidate load(
        const ICandidateFactRepository& repository
    ) const;
};

} // namespace orbit