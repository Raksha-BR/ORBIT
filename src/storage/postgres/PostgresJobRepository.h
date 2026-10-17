#pragma once

#include "storage/IJobRepository.h"
#include "PostgresConnection.h"

namespace orbit {

class PostgresJobRepository final : public IJobRepository {
public:
    explicit PostgresJobRepository(PostgresConnection& connection);

    void save(const Job& job) override;
    std::optional<Job> find(JobId id) const override;
    std::vector<Job> find_all() const override;

private:
    PostgresConnection& connection_;
};

} // namespace orbit
