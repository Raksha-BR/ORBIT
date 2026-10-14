
#include <gtest/gtest.h>

#include "storage/InMemoryJobRepository.h"

namespace orbit {

TEST(InMemoryJobRepositoryTest, SavesAndFindsJob) {
    InMemoryJobRepository repository;

    Job job;
    job.id = 101;
    job.title = "Software Engineer";
    job.company = "Example Company";
    job.location = "Bengaluru";

    repository.save(job);

    const auto result = repository.find(101);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->title, "Software Engineer");
    EXPECT_EQ(result->company, "Example Company");
}

TEST(InMemoryJobRepositoryTest, ReturnsNothingForUnknownJob) {
    InMemoryJobRepository repository;

    EXPECT_FALSE(repository.find(999).has_value());
}

TEST(InMemoryJobRepositoryTest, SavesAndListsMultipleJobs) {
    InMemoryJobRepository repository;

    Job first;
    first.id = 101;
    first.title = "Software Engineer";

    Job second;
    second.id = 102;
    second.title = "Backend Engineer";

    repository.save(first);
    repository.save(second);

    EXPECT_EQ(repository.find_all().size(), 2);
}

TEST(InMemoryJobRepositoryTest, UpdatesJobWithExistingId) {
    InMemoryJobRepository repository;

    Job job;
    job.id = 101;
    job.title = "Software Engineer";

    repository.save(job);

    job.title = "Senior Software Engineer";
    repository.save(job);

    const auto result = repository.find(101);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->title, "Senior Software Engineer");
    EXPECT_EQ(repository.find_all().size(), 1);
}

} // namespace orbit
