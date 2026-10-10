
#include <gtest/gtest.h>

#include "storage/postgres/PostgresApplicationRepository.h"
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

} // namespace

TEST(
    PostgresApplicationRepositoryTest,
    SavesAndFindsApplication
) {
    auto connection = create_connection();
    PostgresSchema::initialize(connection);
    connection.execute("DELETE FROM applications");

    PostgresApplicationRepository repository(connection);

    Application application;
    application.id = 101;
    application.candidate_id = 1;
    application.job_id = 501;
    application.status = ApplicationStatus::Applied;
    application.applied_at = std::chrono::system_clock::now();
    application.last_activity_at = application.applied_at;

    repository.save(application);

    const auto result = repository.find(101);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->id, 101);
    EXPECT_EQ(result->candidate_id, 1);
    EXPECT_EQ(result->job_id, 501);
    EXPECT_EQ(result->status, ApplicationStatus::Applied);
    EXPECT_TRUE(result->applied_at.has_value());
    EXPECT_TRUE(result->last_activity_at.has_value());
}

TEST(
    PostgresApplicationRepositoryTest,
    ReturnsNothingForUnknownApplication
) {
    auto connection = create_connection();
    PostgresSchema::initialize(connection);
    connection.execute("DELETE FROM applications");

    PostgresApplicationRepository repository(connection);

    EXPECT_FALSE(repository.find(999999).has_value());
}

TEST(
    PostgresApplicationRepositoryTest,
    SavesUpdatesAndListsApplications
) {
    auto connection = create_connection();
    PostgresSchema::initialize(connection);
    connection.execute("DELETE FROM applications");

    PostgresApplicationRepository repository(connection);

    Application first;
    first.id = 201;
    first.candidate_id = 1;
    first.job_id = 601;
    first.status = ApplicationStatus::Discovered;

    Application second;
    second.id = 202;
    second.candidate_id = 1;
    second.job_id = 602;
    second.status = ApplicationStatus::Shortlisted;

    repository.save(first);
    repository.save(second);

    // Saving the same ID updates the existing row.
    first.status = ApplicationStatus::Applied;
    first.applied_at = std::chrono::system_clock::now();
    first.last_activity_at = first.applied_at;
    repository.save(first);

    const auto updated = repository.find(201);
    ASSERT_TRUE(updated.has_value());
    EXPECT_EQ(updated->status, ApplicationStatus::Applied);

    const auto applications = repository.find_all();

    ASSERT_EQ(applications.size(), 2);
    EXPECT_EQ(applications[0].id, 201);
    EXPECT_EQ(applications[0].status, ApplicationStatus::Applied);
    EXPECT_EQ(applications[1].id, 202);
    EXPECT_EQ(applications[1].status, ApplicationStatus::Shortlisted);
}

} // namespace orbit
