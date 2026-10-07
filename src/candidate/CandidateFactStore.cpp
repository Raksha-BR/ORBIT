#include "CandidateFactStore.h"

namespace orbit {

void CandidateFactStore::add_fact(
    const CandidateFact& fact
) {
    facts_.push_back(fact);
}

std::optional<CandidateFact> CandidateFactStore::find_fact(
    const std::string& key
) const {
    for (const auto& fact : facts_) {
        if (fact.key == key) {
            return fact;
        }
    }

    return std::nullopt;
}

bool CandidateFactStore::has_fact(
    const std::string& key
) const {
    return find_fact(key).has_value();
}

std::vector<CandidateFact> CandidateFactStore::all_facts() const {
    return facts_;
}

} // namespace orbit