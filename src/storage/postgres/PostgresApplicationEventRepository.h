
#pragma once

#include "../IApplicationEventRepository.h"
#include "PostgresConnection.h"

namespace orbit {

class PostgresApplicationEventRepository
    : public IApplicationEventRepository {
public:
    explicit PostgresApplicationEventRepository(
        PostgresConnection& connection
    );

    void save(
        const ApplicationEvent& event
    ) override;

    std::vector<ApplicationEvent> find_by_application(
        ApplicationId application_id
    ) const override;

private:
    PostgresConnection& connection_;
};

} // namespace orbit
