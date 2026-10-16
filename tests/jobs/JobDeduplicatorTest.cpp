
#include <gtest/gtest.h>

#include "jobs/JobDeduplicator.h"

namespace orbit {

TEST(JobDeduplicatorTest, DetectsSameJobWithDifferentCapitalization) {
    Job first;
    first.company = "Example Corp";
    first.title = "Software Engineer";
    first.location = "Bengaluru";

    Job second;
    second.company = " example corp ";
    second.title = "SOFTWARE ENGINEER";
    second.location = "BENGALURU";

    JobDeduplicator deduplicator;

    EXPECT_TRUE(deduplicator.are_duplicates(first, second));
}

TEST(JobDeduplicatorTest, DetectsMatchingApplicationUrls) {
    Job first;
    first.company = "Company A";
    first.title = "Engineer";
    first.application_url = "https://example.com/jobs/123/";

    Job second;
    second.company = "Company B";
    second.title = "Different title";
    second.application_url = "https://example.com/jobs/123";

    JobDeduplicator deduplicator;

    EXPECT_TRUE(deduplicator.are_duplicates(first, second));
}

TEST(JobDeduplicatorTest, RejectsDifferentJobs) {
    Job first;
    first.company = "Company A";
    first.title = "Software Engineer";
    first.location = "Bengaluru";

    Job second;
    second.company = "Company B";
    second.title = "Backend Engineer";
    second.location = "Bengaluru";

    JobDeduplicator deduplicator;

    EXPECT_FALSE(deduplicator.are_duplicates(first, second));
}

TEST(JobDeduplicatorTest, DoesNotMatchEmptyJobs) {
    Job first;
    Job second;

    JobDeduplicator deduplicator;

    EXPECT_FALSE(deduplicator.are_duplicates(first, second));
}

} // namespace orbit
