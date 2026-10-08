#include "PostgresCandidateFactRepository.h"

namespace orbit {

PostgresCandidateFactRepository::
    PostgresCandidateFactRepository(
        PostgresConnection& connection
    )
    : connection_(connection) {
}

void PostgresCandidateFactRepository::save(
    const CandidateFact& fact
) {
    // Database implementation will be added
    // in the next persistence slice.
    (void)fact;
}

std::optional<CandidateFact>
PostgresCandidateFactRepository::find(
    const std::string& key
) const {
    (void)key;

    return std::nullopt;
}

std::vector<CandidateFact>
PostgresCandidateFactRepository::find_all()
    const {
    return {};
}

} // namespace orbit