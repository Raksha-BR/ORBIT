#include "InMemoryCandidateFactRepository.h"
#include <algorithm>

namespace orbit {

void InMemoryCandidateFactRepository::save(
    const CandidateFact& fact
) {
    facts_.push_back(fact);
}

void InMemoryCandidateFactRepository::replace(
    const CandidateFact& fact
) {
    facts_.erase(
        std::remove_if(
            facts_.begin(),
            facts_.end(),
            [&fact](const CandidateFact& existing) {
                return existing.key == fact.key;
            }
        ),
        facts_.end()
    );

    facts_.push_back(fact);
}

std::optional<CandidateFact>
InMemoryCandidateFactRepository::find(
    const std::string& key
) const {

    for (const auto& fact : facts_) {
        if (fact.key == key) {
            return fact;
        }
    }

    return std::nullopt;
}

std::vector<CandidateFact>
InMemoryCandidateFactRepository::find_all() const {
    return facts_;
}

} // namespace orbit