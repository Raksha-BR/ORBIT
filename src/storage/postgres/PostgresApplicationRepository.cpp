
#include "PostgresApplicationRepository.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace orbit {

namespace {

std::string to_string(ApplicationStatus status) {
    switch (status) {
        case ApplicationStatus::Discovered: return "DISCOVERED";
        case ApplicationStatus::Shortlisted: return "SHORTLISTED";
        case ApplicationStatus::Preparing: return "PREPARING";
        case ApplicationStatus::ReadyForReview: return "READY_FOR_REVIEW";
        case ApplicationStatus::Approved: return "APPROVED";
        case ApplicationStatus::Applying: return "APPLYING";
        case ApplicationStatus::Applied: return "APPLIED";
        case ApplicationStatus::Assessment: return "ASSESSMENT";
        case ApplicationStatus::Interview: return "INTERVIEW";
        case ApplicationStatus::Offer: return "OFFER";
        case ApplicationStatus::Rejected: return "REJECTED";
        case ApplicationStatus::Withdrawn: return "WITHDRAWN";
        case ApplicationStatus::Closed: return "CLOSED";
    }

    throw std::invalid_argument("Unknown application status");
}

ApplicationStatus status_from_string(const std::string& status) {
    if (status == "DISCOVERED") return ApplicationStatus::Discovered;
    if (status == "SHORTLISTED") return ApplicationStatus::Shortlisted;
    if (status == "PREPARING") return ApplicationStatus::Preparing;
    if (status == "READY_FOR_REVIEW") return ApplicationStatus::ReadyForReview;
    if (status == "APPROVED") return ApplicationStatus::Approved;
    if (status == "APPLYING") return ApplicationStatus::Applying;
    if (status == "APPLIED") return ApplicationStatus::Applied;
    if (status == "ASSESSMENT") return ApplicationStatus::Assessment;
    if (status == "INTERVIEW") return ApplicationStatus::Interview;
    if (status == "OFFER") return ApplicationStatus::Offer;
    if (status == "REJECTED") return ApplicationStatus::Rejected;
    if (status == "WITHDRAWN") return ApplicationStatus::Withdrawn;
    if (status == "CLOSED") return ApplicationStatus::Closed;

    throw std::runtime_error("Unknown application status in database: " + status);
}

std::string time_to_epoch(
    const std::chrono::system_clock::time_point& time
) {
    const double seconds =
        std::chrono::duration<double>(
            time.time_since_epoch()
        ).count();

    std::ostringstream output;
    output << std::setprecision(17) << seconds;
    return output.str();
}

std::chrono::system_clock::time_point time_from_epoch(
    const std::string& value
) {
    const double seconds = std::stod(value);

    return std::chrono::system_clock::time_point{
        std::chrono::duration_cast<
            std::chrono::system_clock::duration
        >(std::chrono::duration<double>(seconds))
    };
}

std::string optional_time_to_epoch(
    const std::optional<std::chrono::system_clock::time_point>& time
) {
    return time.has_value() ? time_to_epoch(*time) : "";
}

std::optional<std::chrono::system_clock::time_point>
optional_time_from_epoch(const std::string& value) {
    if (value.empty()) {
        return std::nullopt;
    }

    return time_from_epoch(value);
}

Application application_from_row(
    const std::vector<std::string>& row
) {
    Application application;

    application.id = std::stoull(row[0]);
    application.candidate_id = std::stoull(row[1]);
    application.job_id = std::stoull(row[2]);
    application.status = status_from_string(row[3]);
    application.created_at = time_from_epoch(row[4]);
    application.applied_at = optional_time_from_epoch(row[5]);
    application.last_activity_at = optional_time_from_epoch(row[6]);

    return application;
}

} // namespace

PostgresApplicationRepository::PostgresApplicationRepository(
    PostgresConnection& connection
)
    : connection_(connection) {
}

void PostgresApplicationRepository::save(
    const Application& application
) {
    connection_.execute(
        R"SQL(
            INSERT INTO applications (
                id,
                candidate_id,
                job_id,
                status,
                created_at,
                applied_at,
                last_activity_at
            )
            VALUES (
                $1::bigint,
                $2::bigint,
                $3::bigint,
                $4,
                to_timestamp($5::double precision),
                CASE
                    WHEN $6 = '' THEN NULL
                    ELSE to_timestamp($6::double precision)
                END,
                CASE
                    WHEN $7 = '' THEN NULL
                    ELSE to_timestamp($7::double precision)
                END
            )
            ON CONFLICT (id) DO UPDATE SET
                candidate_id = EXCLUDED.candidate_id,
                job_id = EXCLUDED.job_id,
                status = EXCLUDED.status,
                created_at = EXCLUDED.created_at,
                applied_at = EXCLUDED.applied_at,
                last_activity_at = EXCLUDED.last_activity_at
        )SQL",
        {
            std::to_string(application.id),
            std::to_string(application.candidate_id),
            std::to_string(application.job_id),
            to_string(application.status),
            time_to_epoch(application.created_at),
            optional_time_to_epoch(application.applied_at),
            optional_time_to_epoch(application.last_activity_at)
        }
    );
}

std::optional<Application> PostgresApplicationRepository::find(
    ApplicationId id
) const {
    const auto rows = connection_.query(
        R"SQL(
            SELECT
                id::text,
                candidate_id::text,
                job_id::text,
                status,
                extract(epoch FROM created_at)::text,
                COALESCE(extract(epoch FROM applied_at)::text, ''),
                COALESCE(extract(epoch FROM last_activity_at)::text, '')
            FROM applications
            WHERE id = $1::bigint
        )SQL",
        {std::to_string(id)}
    );

    if (rows.empty()) {
        return std::nullopt;
    }

    return application_from_row(rows.front());
}

std::vector<Application>
PostgresApplicationRepository::find_all() const {
    const auto rows = connection_.query(
        R"SQL(
            SELECT
                id::text,
                candidate_id::text,
                job_id::text,
                status,
                extract(epoch FROM created_at)::text,
                COALESCE(extract(epoch FROM applied_at)::text, ''),
                COALESCE(extract(epoch FROM last_activity_at)::text, '')
            FROM applications
            ORDER BY created_at, id
        )SQL"
    );

    std::vector<Application> applications;
    applications.reserve(rows.size());

    for (const auto& row : rows) {
        applications.push_back(application_from_row(row));
    }

    return applications;
}

} // namespace orbit
