
#include "jobs/JobSearchService.h"
#include "jobs/JobDeduplicator.h"
#include "storage/IJobRepository.h"

#include <gtest/gtest.h>

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace orbit {
namespace {

class FakeJobRepository final : public IJobRepository {
public:
    void save(const Job& job) override {
        jobs.push_back(job);
    }

    std::optional<Job> find(JobId id) const override {
        for (const auto& job : jobs) {
            if (job.id == id) {
                return job;
            }
        }
        return std::nullopt;
    }

    std::vector<Job> find_all() const override {
        return jobs;
    }

    std::vector<Job> jobs;
};

Job make_job(
    JobId id,
    std::string title,
    std::string company,
    std::string url
) {
    Job job;
    job.id = id;
    job.title = std::move(title);
    job.company = std::move(company);
    job.location = "Bengaluru";
    job.application_url = url;
    job.original_url = url;
    job.source_name = "Test";
    return job;
}

TEST(JobSearchServiceTest, SavesNewJobs) {
    FakeJobRepository repository;
    JobDeduplicator deduplicator;
    JobSearchService service(repository, deduplicator);

    auto saved = service.save_jobs({
        make_job(1, "Software Engineer", "Example A", "https://example.com/1"),
        make_job(2, "Backend Engineer", "Example B", "https://example.com/2")
    });

    EXPECT_EQ(saved.size(), 2U);
    EXPECT_EQ(repository.jobs.size(), 2U);
}

TEST(JobSearchServiceTest, SkipsJobsAlreadyInRepository) {
    FakeJobRepository repository;
    JobDeduplicator deduplicator;

    repository.save(
        make_job(1, "Software Engineer", "Example", "https://example.com/job")
    );

    JobSearchService service(repository, deduplicator);

    auto saved = service.save_jobs({
        make_job(2, "Software Engineer", "Example", "https://example.com/job")
    });

    EXPECT_TRUE(saved.empty());
    EXPECT_EQ(repository.jobs.size(), 1U);
}

TEST(JobSearchServiceTest, SkipsDuplicatesWithinIncomingBatch) {
    FakeJobRepository repository;
    JobDeduplicator deduplicator;
    JobSearchService service(repository, deduplicator);

    auto saved = service.save_jobs({
        make_job(1, "Software Engineer", "Example", "https://example.com/job"),
        make_job(2, "Software Engineer", "Example", "https://example.com/job")
    });

    EXPECT_EQ(saved.size(), 1U);
    EXPECT_EQ(repository.jobs.size(), 1U);
}

TEST(JobSearchServiceTest, SearchesKeywordsCaseInsensitively) {
    FakeJobRepository repository;
    JobDeduplicator deduplicator;

    repository.save(
        make_job(1, "Software Engineer", "Example", "https://example.com/1")
    );
    repository.save(
        make_job(2, "Product Designer", "Other", "https://example.com/2")
    );

    JobSearchService service(repository, deduplicator);
    const auto results = service.search_by_keyword("sOfTwArE");

    ASSERT_EQ(results.size(), 1U);
    EXPECT_EQ(results.front().title, "Software Engineer");
}

TEST(JobSearchServiceTest, ReturnsNoResultsForEmptyKeyword) {
    FakeJobRepository repository;
    JobDeduplicator deduplicator;
    JobSearchService service(repository, deduplicator);

    EXPECT_TRUE(service.search_by_keyword("").empty());
    EXPECT_TRUE(service.search_by_keyword("   ").empty());
    EXPECT_TRUE(service.search_by_keyword("\t\n").empty());
}

} // namespace
} // namespace orbit
