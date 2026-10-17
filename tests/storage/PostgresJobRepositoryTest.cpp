#include <gtest/gtest.h>

#include "core/Job.h"
#include "storage/postgres/PostgresConnection.h"
#include "storage/postgres/PostgresJobRepository.h"
#include "storage/postgres/PostgresSchema.h"

namespace orbit {
namespace {

PostgresConnection create_connection() {
    return PostgresConnection(
        "host=localhost port=5432 dbname=orbit "
        "user=orbit_user password=orbit_dev_password"
    );
}

void clean_jobs(PostgresConnection& connection) {
    connection.execute("DELETE FROM jobs");
}

Job make_job(JobId id = 90001) {
    Job job;
    job.id = id;
    job.title = "Software Engineer";
    job.company = "Example Corp";
    job.location = "Bengaluru";
    job.salary = "15-20 LPA";
    job.minimum_years_experience = 1;
    job.description = "Backend software development";
    job.application_url = "https://example.com/jobs/90001";
    job.skills = {"C++", "Python", "PostgreSQL"};
    return job;
}

TEST(PostgresJobRepositoryTest, SavesAndFindsJobWithSkills) {
    auto connection = create_connection();
    PostgresSchema::initialize(connection);
    clean_jobs(connection);

    PostgresJobRepository repository(connection);
    const Job original = make_job();

    repository.save(original);

    const auto result = repository.find(original.id);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->id, original.id);
    EXPECT_EQ(result->title, original.title);
    EXPECT_EQ(result->company, original.company);
    EXPECT_EQ(result->location, original.location);
    EXPECT_EQ(result->salary, original.salary);
    EXPECT_EQ(
        result->minimum_years_experience,
        original.minimum_years_experience
    );
    EXPECT_EQ(result->description, original.description);
    EXPECT_EQ(result->application_url, original.application_url);
    EXPECT_EQ(result->skills, original.skills);

    clean_jobs(connection);
}

TEST(PostgresJobRepositoryTest, ReturnsNothingForUnknownJob) {
    auto connection = create_connection();
    PostgresSchema::initialize(connection);
    clean_jobs(connection);

    PostgresJobRepository repository(connection);

    EXPECT_FALSE(repository.find(99999999).has_value());
}

TEST(PostgresJobRepositoryTest, UpdatesExistingJobAndReplacesSkills) {
    auto connection = create_connection();
    PostgresSchema::initialize(connection);
    clean_jobs(connection);

    PostgresJobRepository repository(connection);

    Job job = make_job();
    repository.save(job);

    job.title = "Senior Software Engineer";
    job.company = "Updated Corp";
    job.skills = {"C++", "Linux"};

    repository.save(job);

    const auto result = repository.find(job.id);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->title, "Senior Software Engineer");
    EXPECT_EQ(result->company, "Updated Corp");
    ASSERT_EQ(result->skills.size(), 2u);
    EXPECT_EQ(result->skills[0], "C++");
    EXPECT_EQ(result->skills[1], "Linux");

    clean_jobs(connection);
}

TEST(PostgresJobRepositoryTest, SavesAndListsMultipleJobs) {
    auto connection = create_connection();
    PostgresSchema::initialize(connection);
    clean_jobs(connection);

    PostgresJobRepository repository(connection);

    repository.save(make_job(90002));
    repository.save(make_job(90001));

    const auto jobs = repository.find_all();

    ASSERT_EQ(jobs.size(), 2u);

    // find_all() returns jobs ordered by ID.
    EXPECT_EQ(jobs[0].id, 90001);
    EXPECT_EQ(jobs[1].id, 90002);

    EXPECT_EQ(jobs[0].skills.size(), 3u);
    EXPECT_EQ(jobs[1].skills.size(), 3u);

    clean_jobs(connection);
}

} // namespace
} // namespace orbit
