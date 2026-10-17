#pragma once

#include "storage/IJobRepository.h"
#include "PostgresConnection.h"
#include "storage/Page.h"
#include <cstddef>

namespace orbit {

class PostgresJobRepository final : public IJobRepository {
public:
    explicit PostgresJobRepository(PostgresConnection& connection);

    void save(const Job& job) override;
    std::optional<Job> find(JobId id) const override;
    std::vector<Job> find_all() const override;
    Page<Job> find_page(std::size_t offset, std::size_t limit) const;

private:
    PostgresConnection& connection_;
};

} // namespace orbit
