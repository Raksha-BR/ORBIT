#include "jobs/JobSourceService.h"

#include <exception>
#include <utility>

namespace orbit {

JobSourceService::JobSourceService(
    JobSearchService& search_service
)
    : search_service_(search_service) {
}

JobSourceResult JobSourceService::collect(IJobSource& source) {
    JobSourceResult result;
    result.source_name = source.name();

    try {
        auto jobs = source.fetch_jobs();
        result.fetched_count = jobs.size();

        const auto existing_jobs = search_service_.get_all_jobs();

        JobId next_id = 1;

        for (const auto& existing : existing_jobs) {
            if (existing.id >= next_id) {
                next_id = existing.id + 1;
            }
        }

        for (auto& job : jobs) {
            job.id = next_id++;

            if (job.source_name.empty()) {
                job.source_name = result.source_name;
            }
        }

        auto saved_jobs =
            search_service_.save_jobs(std::move(jobs));

        result.rejected_count =
            result.fetched_count - saved_jobs.size();

        result.jobs = std::move(saved_jobs);
    } catch (const std::exception& error) {
        result.error = error.what();
    }

    return result;
}

} // namespace orbit