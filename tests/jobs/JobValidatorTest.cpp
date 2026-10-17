#include <gtest/gtest.h>

#include "jobs/JobValidator.h"

namespace orbit {
namespace {

Job make_valid_job() {
    Job job;
    job.id = 1001;
    job.title = "Software Engineer";
    job.company = "Example Corp";
    job.location = "Bengaluru";
    job.description = "Backend development";
    return job;
}

TEST(JobValidatorTest, AcceptsValidJob) {
    const auto errors = JobValidator::validate(make_valid_job());
    EXPECT_TRUE(errors.empty());
}

TEST(JobValidatorTest, RejectsMissingTitle) {
    auto job = make_valid_job();
    job.title = "";

    const auto errors = JobValidator::validate(job);

    EXPECT_FALSE(errors.empty());
    EXPECT_EQ(errors.front(), "Job title is required");
}

TEST(JobValidatorTest, RejectsMissingCompany) {
    auto job = make_valid_job();
    job.company = "  ";

    const auto errors = JobValidator::validate(job);

    EXPECT_FALSE(errors.empty());
}

TEST(JobValidatorTest, RejectsNegativeExperience) {
    auto job = make_valid_job();
    job.minimum_years_experience = -1;

    const auto errors = JobValidator::validate(job);

    EXPECT_FALSE(errors.empty());
}

} // namespace
} // namespace orbit