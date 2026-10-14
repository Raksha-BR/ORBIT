
#include <gtest/gtest.h>

#include "applications/ApplicationWorkflowService.h"
#include "storage/postgres/PostgresApplicationRepository.h"
#include "storage/postgres/PostgresApplicationEventRepository.h"
#include "storage/postgres/PostgresConnection.h"
#include "storage/postgres/PostgresSchema.h"

#include <stdexcept>
#include <string>
#include <vector>

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

// Deliberately fails to simulate a database error while saving an event.
class FailingApplicationEventRepository
    : public IApplicationEventRepository {
public:
    explicit FailingApplicationEventRepository(
        PostgresConnection& connection
    )
        : connection_(connection) {
    }

    void save(const ApplicationEvent&) override {
        connection_.execute(
            "INSERT INTO orbit_intentionally_missing_table "
            "(id) VALUES (1)"
        );
    }

    std::vector<ApplicationEvent> find_by_application(
        ApplicationId
    ) const override {
        return {};
    }

private:
    PostgresConnection& connection_;
};

TEST(
    PostgresApplicationWorkflowTransactionTest,
    RollsBackApplicationUpdateWhenEventInsertFails
) {
    auto connection = create_connection();
    PostgresSchema::initialize(connection);

    // Intended for the dedicated local ORBIT development database.
    connection.execute("DELETE FROM applications");

    PostgresApplicationRepository applications(connection);
    PostgresApplicationEventRepository stored_events(connection);
    FailingApplicationEventRepository failing_events(connection);

    Application application;
    application.id = 903;
    application.candidate_id = 1;
    application.job_id = 903;
    application.status = ApplicationStatus::Discovered;

    applications.save(application);

    // All three components use the same PostgreSQL connection.
    ApplicationWorkflowService workflow(
        applications,
        failing_events,
        connection
    );

    EXPECT_ANY_THROW(
        workflow.transition(
            903,
            ApplicationStatus::Shortlisted
        )
    );

    // The status update must have been rolled back.
    const auto stored_application = applications.find(903);

    ASSERT_TRUE(stored_application.has_value());
    EXPECT_EQ(
        stored_application->status,
        ApplicationStatus::Discovered
    );

    // No event should have been recorded.
    EXPECT_TRUE(
        stored_events.find_by_application(903).empty()
    );
}

} // namespace
} // namespace orbit

