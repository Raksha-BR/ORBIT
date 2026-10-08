#include "InMemoryCandidateFactRepository.h"

namespace orbit {

void InMemoryCandidateFactRepository::save(
    const CandidateFact& fact
) {
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