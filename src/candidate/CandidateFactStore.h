#pragma once

#include "CandidateFact.h"

#include <optional>
#include <string>
#include <vector>

namespace orbit {

class CandidateFactStore {
public:
    void add_fact(const CandidateFact& fact);

    std::optional<CandidateFact> find_fact(
        const std::string& key
    ) const;

    bool has_fact(
        const std::string& key
    ) const;

    std::vector<CandidateFact> all_facts() const;

private:
    std::vector<CandidateFact> facts_;
};

} // namespace orbit