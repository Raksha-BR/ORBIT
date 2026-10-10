
#pragma once

#include "../IApplicationRepository.h"
#include "PostgresConnection.h"

namespace orbit {

class PostgresApplicationRepository
    : public IApplicationRepository {
public:
    explicit PostgresApplicationRepository(
        PostgresConnection& connection
    );

    void save(
        const Application& application
    ) override;

    std::optional<Application> find(
        ApplicationId id
    ) const override;

    std::vector<Application> find_all() const override;

private:
    PostgresConnection& connection_;
};

} // namespace orbit
