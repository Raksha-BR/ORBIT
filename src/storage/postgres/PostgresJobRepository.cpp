
#include "PostgresJobRepository.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace orbit {
namespace {

std::string optional_string(
    const std::optional<std::string>& value
) {
    return value.value_or("");
}

std::string optional_int(
    const std::optional<int>& value
) {
    return value ? std::to_string(*value) : "";
}

Job job_from_row(const std::vector<std::string>& row) {
    Job job;

    job.id = static_cast<JobId>(std::stoll(row.at(0)));
    job.title = row.at(1);
    job.company = row.at(2);
    job.location = row.at(3);

    if (!row.at(4).empty()) {
        job.salary = row.at(4);
    }

    if (!row.at(5).empty()) {
        job.minimum_years_experience = std::stoi(row.at(5));
    }

    job.description = row.at(6);
    job.application_url = row.at(7);
    job.source_name = row.at(8);
    job.external_job_id = row.at(9);
    job.original_url = row.at(10);

    return job;
}

constexpr const char* kSelectJobs = R"SQL(
    SELECT
        id,
        title,
        company,
        location,
        COALESCE(salary, ''),
        COALESCE(minimum_years_experience::TEXT, ''),
        description,
        application_url,
        COALESCE(source_name, ''),
        COALESCE(external_job_id, ''),
        COALESCE(original_url, '')
    FROM jobs
)SQL";

void load_skills(
    PostgresConnection& connection,
    std::vector<Job>& jobs
) {
    if (jobs.empty()) {
        return;
    }

    const auto rows = connection.query(
        R"SQL(
            SELECT job_id, skill
            FROM job_skills
            ORDER BY job_id, position
        )SQL"
    );

    std::unordered_map<JobId, std::size_t> indexes;
    indexes.reserve(jobs.size());

    for (std::size_t i = 0; i < jobs.size(); ++i) {
        indexes.emplace(jobs[i].id, i);
    }

    for (const auto& row : rows) {
        const auto id = static_cast<JobId>(std::stoll(row.at(0)));
        const auto found = indexes.find(id);

        if (found != indexes.end()) {
            jobs[found->second].skills.push_back(row.at(1));
        }
    }
}

} // namespace

PostgresJobRepository::PostgresJobRepository(
    PostgresConnection& connection
)
    : connection_(connection) {
}

void PostgresJobRepository::save(const Job& job) {
    connection_.begin_transaction();

    try {
        connection_.execute(
            R"SQL(
                INSERT INTO jobs (
                    id,
                    title,
                    company,
                    location,
                    salary,
                    minimum_years_experience,
                    description,
                    application_url,
                    source_name,
                    external_job_id,
                    original_url
                )
                VALUES (
                    $1, $2, $3, $4,
                    NULLIF($5, ''),
                    NULLIF($6, '')::INTEGER,
                    $7, $8, $9, $10, $11
                )
                ON CONFLICT (id) DO UPDATE SET
                    title = EXCLUDED.title,
                    company = EXCLUDED.company,
                    location = EXCLUDED.location,
                    salary = EXCLUDED.salary,
                    minimum_years_experience =
                        EXCLUDED.minimum_years_experience,
                    description = EXCLUDED.description,
                    application_url = EXCLUDED.application_url,
                    source_name = EXCLUDED.source_name,
                    external_job_id = EXCLUDED.external_job_id,
                    original_url = EXCLUDED.original_url
            )SQL",
            {
                std::to_string(job.id),
                job.title,
                job.company,
                job.location,
                optional_string(job.salary),
                optional_int(job.minimum_years_experience),
                job.description,
                job.application_url,
                job.source_name,
                job.external_job_id,
                job.original_url
            }
        );

        connection_.execute(
            "DELETE FROM job_skills WHERE job_id = $1",
            {std::to_string(job.id)}
        );

        for (std::size_t i = 0; i < job.skills.size(); ++i) {
            connection_.execute(
                R"SQL(
                    INSERT INTO job_skills (
                        job_id,
                        position,
                        skill
                    )
                    VALUES ($1, $2, $3)
                )SQL",
                {
                    std::to_string(job.id),
                    std::to_string(i),
                    job.skills[i]
                }
            );
        }

        connection_.commit_transaction();
    } catch (...) {
        connection_.rollback_transaction();
        throw;
    }
}

std::optional<Job> PostgresJobRepository::find(JobId id) const {
    auto rows = connection_.query(
        std::string(kSelectJobs) + " WHERE id = $1",
        {std::to_string(id)}
    );

    if (rows.empty()) {
        return std::nullopt;
    }

    std::vector<Job> jobs;
    jobs.push_back(job_from_row(rows.front()));
    load_skills(connection_, jobs);

    return jobs.front();
}

std::vector<Job> PostgresJobRepository::find_all() const {
    const auto rows = connection_.query(
        std::string(kSelectJobs) + " ORDER BY id"
    );

    std::vector<Job> jobs;
    jobs.reserve(rows.size());

    for (const auto& row : rows) {
        jobs.push_back(job_from_row(row));
    }

    load_skills(connection_, jobs);
    return jobs;
}

Page<Job> PostgresJobRepository::find_page(
    std::size_t offset,
    std::size_t limit
) const {
    if (limit == 0 || limit > 100) {
        throw std::invalid_argument(
            "Page limit must be between 1 and 100"
        );
    }

    const auto rows = connection_.query(
        std::string(kSelectJobs) +
            " ORDER BY id LIMIT $1 OFFSET $2",
        {
            std::to_string(limit + 1),
            std::to_string(offset)
        }
    );

    Page<Job> page;
    page.offset = offset;
    page.limit = limit;
    page.has_more = rows.size() > limit;

    const std::size_t count =
        page.has_more ? limit : rows.size();

    std::vector<Job> jobs;
    jobs.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        jobs.push_back(job_from_row(rows[i]));
    }

    load_skills(connection_, jobs);
    page.items = std::move(jobs);

    return page;
}

} // namespace orbit
