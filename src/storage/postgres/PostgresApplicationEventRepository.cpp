
#include "PostgresApplicationEventRepository.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <stdexcept>

namespace orbit {

namespace {

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

} // namespace

PostgresApplicationEventRepository::
    PostgresApplicationEventRepository(
        PostgresConnection& connection
    )
    : connection_(connection) {
}

void PostgresApplicationEventRepository::save(
    const ApplicationEvent& event
) {
    const auto occurred_at = time_to_epoch(event.occurred_at);

    if (event.id == 0) {
        connection_.execute(
            R"SQL(
                INSERT INTO application_events (
                    application_id,
                    event_type,
                    description,
                    occurred_at
                )
                VALUES (
                    $1::bigint,
                    $2,
                    $3,
                    to_timestamp($4::double precision)
                )
            )SQL",
            {
                std::to_string(event.application_id),
                event.event_type,
                event.description,
                occurred_at
            }
        );
    } else {
        connection_.execute(
            R"SQL(
                INSERT INTO application_events (
                    id,
                    application_id,
                    event_type,
                    description,
                    occurred_at
                )
                VALUES (
                    $1::bigint,
                    $2::bigint,
                    $3,
                    $4,
                    to_timestamp($5::double precision)
                )
            )SQL",
            {
                std::to_string(event.id),
                std::to_string(event.application_id),
                event.event_type,
                event.description,
                occurred_at
            }
        );
    }
}

std::vector<ApplicationEvent>
PostgresApplicationEventRepository::find_by_application(
    ApplicationId application_id
) const {
    const auto rows = connection_.query(
        R"SQL(
            SELECT
                id::text,
                application_id::text,
                event_type,
                description,
                extract(epoch FROM occurred_at)::text
            FROM application_events
            WHERE application_id = $1::bigint
            ORDER BY occurred_at, id
        )SQL",
        {std::to_string(application_id)}
    );

    std::vector<ApplicationEvent> events;
    events.reserve(rows.size());

    for (const auto& row : rows) {
        ApplicationEvent event;

        event.id = std::stoull(row[0]);
        event.application_id = std::stoull(row[1]);
        event.event_type = row[2];
        event.description = row[3];
        event.occurred_at = time_from_epoch(row[4]);

        events.push_back(event);
    }

    return events;
}

} // namespace orbit
