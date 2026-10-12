
#include <gtest/gtest.h>

#include "storage/postgres/PostgresApplicationRepository.h"
#include "storage/postgres/PostgresApplicationEventRepository.h"
#include "storage/postgres/PostgresConnection.h"
#include "storage/postgres/PostgresSchema.h"

namespace orbit {

namespace {

PostgresConnection create_connection() {
    return PostgresConnection(
        "host=localhost "
        "port=5432 "
        "dbname=orbit "
        "user=orbit_user "
        "password=orbit_dev_password"
    );
}

void clear_test_data(PostgresConnection& connection) {
    connection.execute("DELETE FROM applications");
}

} // namespace

TEST(
    PostgresApplicationEventRepositoryTest,
    SavesAndRetrievesEventsInChronologicalOrder
) {
    auto connection = create_connection();
    PostgresSchema::initialize(connection);
    clear_test_data(connection);

    PostgresApplicationRepository applications(connection);
    PostgresApplicationEventRepository events(connection);

    Application application;
    application.id = 901;
    application.candidate_id = 1;
    application.job_id = 901;
    applications.save(application);

    const auto earlier =
        std::chrono::system_clock::now() - std::chrono::hours(2);
    const auto later =
        std::chrono::system_clock::now();

    events.save(
        ApplicationEvent{
            .application_id = 901,
            .event_type = "APPLIED",
            .description = "Application submitted",
            .occurred_at = later
        }
    );

    events.save(
        ApplicationEvent{
            .application_id = 901,
            .event_type = "SHORTLISTED",
            .description = "Job shortlisted",
            .occurred_at = earlier
        }
    );

    const auto timeline = events.find_by_application(901);

    ASSERT_EQ(timeline.size(), 2);
    EXPECT_EQ(timeline[0].event_type, "SHORTLISTED");
    EXPECT_EQ(timeline[1].event_type, "APPLIED");
    EXPECT_EQ(timeline[0].application_id, 901);
}

TEST(
    PostgresApplicationEventRepositoryTest,
    ReturnsEmptyForApplicationWithoutEvents
) {
    auto connection = create_connection();
    PostgresSchema::initialize(connection);
    clear_test_data(connection);

    PostgresApplicationRepository applications(connection);
    PostgresApplicationEventRepository events(connection);

    Application application;
    application.id = 902;
    application.candidate_id = 1;
    application.job_id = 902;
    applications.save(application);

    EXPECT_TRUE(
        events.find_by_application(902).empty()
    );
}

} // namespace orbit
