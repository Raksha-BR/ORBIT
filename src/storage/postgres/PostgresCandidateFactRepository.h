#pragma once

#include "../IRepository.h"
#include "PostgresConnection.h"

namespace orbit {

class PostgresCandidateFactRepository
    : public ICandidateFactRepository {

public:
    explicit PostgresCandidateFactRepository(
        PostgresConnection& connection
    );

    void save(
        const CandidateFact& fact
    ) override;

    std::optional<CandidateFact> find(
        const std::string& key
    ) const override;

    std::vector<CandidateFact> find_all()
        const override;

private:
    PostgresConnection& connection_;
};

} // namespace orbit