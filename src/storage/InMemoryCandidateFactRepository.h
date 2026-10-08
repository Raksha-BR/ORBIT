#pragma once

#include "IRepository.h"

namespace orbit {

class InMemoryCandidateFactRepository
    : public ICandidateFactRepository {

public:
    void save(
        const CandidateFact& fact
    ) override;

    std::optional<CandidateFact> find(
        const std::string& key
    ) const override;

    std::vector<CandidateFact> find_all() const override;

private:
    std::vector<CandidateFact> facts_;
};

} // namespace orbit