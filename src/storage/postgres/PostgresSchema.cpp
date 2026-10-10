#include "PostgresSchema.h"

#include "PostgresConnection.h"

namespace orbit {

void PostgresSchema::initialize(
    PostgresConnection& connection
) {
    connection.execute(
        R"SQL(
            CREATE TABLE IF NOT EXISTS candidate_facts (
                id BIGSERIAL PRIMARY KEY,

                fact_key TEXT NOT NULL,
                fact_value TEXT NOT NULL,

                confidence TEXT NOT NULL,

                created_at TIMESTAMPTZ NOT NULL
                    DEFAULT CURRENT_TIMESTAMP
            );

        )SQL"
    );
        connection.execute(
        R"SQL(
            CREATE TABLE IF NOT EXISTS applications (
                id BIGINT PRIMARY KEY,
                candidate_id BIGINT NOT NULL,
                job_id BIGINT NOT NULL,
                status TEXT NOT NULL,
                created_at TIMESTAMPTZ NOT NULL,
                applied_at TIMESTAMPTZ,
                last_activity_at TIMESTAMPTZ
            );
        )SQL"
    );
}

} // namespace orbit