#pragma once

#include "core/Job.h"
#include "storage/IJobRepository.h"

#include <cstddef>
#include <vector>

namespace orbit {

class JobDeduplicator;

class JobSearchService {
public:
    JobSearchService(
        IJobRepository& repository,
        JobDeduplicator& deduplicator
    );

    std::vector<Job> save_jobs(std::vector<Job> jobs);
    std::vector<Job> get_all_jobs() const;
    std::vector<Job> search_by_keyword(const std::string& keyword) const;

private:
    IJobRepository& repository_;
    JobDeduplicator& deduplicator_;
};

} // namespace orbit