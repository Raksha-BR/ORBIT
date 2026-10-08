#pragma once

#include "../candidate/CandidateFact.h"

#include <optional>
#include <string>
#include <vector>

namespace orbit {

class ICandidateFactRepository {
public:
    virtual ~ICandidateFactRepository() = default;

    virtual void save(
        const CandidateFact& fact
    ) = 0;

    virtual std::optional<CandidateFact> find(
        const std::string& key
    ) const = 0;

    virtual std::vector<CandidateFact> find_all() const = 0;
};

} // namespace orbit