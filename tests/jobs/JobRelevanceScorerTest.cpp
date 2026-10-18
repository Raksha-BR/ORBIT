
#include "jobs/JobRelevanceScorer.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace orbit {
namespace {

Job make_job(
    std::string title,
    std::string location = "Bengaluru",
    std::string description = "Backend software development"
) {
    Job job;
    job.title = std::move(title);
    job.location = std::move(location);
    job.description = std::move(description);
    return job;
}

TEST(JobRelevanceScorerTest, PrioritizesBackendSoftwareRoles) {
    JobRelevanceScorer scorer;

    Job job = make_job(
        "Software Engineer",
        "Bengaluru",
        "Build backend services in C++ and Python on Linux"
    );

    job.skills = {"C++", "Python", "Linux"};
    job.minimum_years_experience = 2;
    job.salary = "18 LPA";

    const auto result = scorer.score(job);

    EXPECT_GE(result.score, 75);
    EXPECT_LE(result.score, 100);
    EXPECT_FALSE(result.reasons.empty());
}

TEST(JobRelevanceScorerTest, PenalizesIrrelevantRoles) {
    JobRelevanceScorer scorer;

    Job job = make_job(
        "Sales Manager",
        "London",
        "Manage customer accounts and sales targets"
    );

    const auto result = scorer.score(job);

    EXPECT_LT(result.score, 30);
}


TEST(JobRelevanceScorerTest, DoesNotPenalizeUnknownSalaryOrExperience) {
    JobRelevanceScorer scorer;

    Job job = make_job(
        "Software Engineer",
        "Remote",
        "Develop Python backend APIs"
    );

    job.salary = "10 LPA";
    job.minimum_years_experience = 5;

    const auto known_requirements = scorer.score(job);

    job.salary = std::nullopt;
    job.minimum_years_experience = std::nullopt;

    const auto unknown_requirements = scorer.score(job);

    EXPECT_GT(
        unknown_requirements.score,
        known_requirements.score
    );
}


TEST(JobRelevanceScorerTest, RewardsPreferredLocationAndRemoteWork) {
    JobRelevanceScorer scorer;

    Job bengaluru = make_job("Software Engineer", "Bengaluru");
    Job remote = make_job("Software Engineer", "Remote");
    Job other = make_job("Software Engineer", "London");

    const auto bengaluru_score = scorer.score(bengaluru).score;
    const auto remote_score = scorer.score(remote).score;
    const auto other_score = scorer.score(other).score;

    EXPECT_GT(bengaluru_score, other_score);
    EXPECT_GT(remote_score, other_score);
}

TEST(JobRelevanceScorerTest, ScoreStaysWithinZeroAndOneHundred) {
    JobRelevanceScorer scorer;

    Job job = make_job(
        "Junior Software Engineer",
        "Bengaluru",
        "C++ Python backend distributed systems REST API Linux"
    );

    job.skills = {"C++", "Python", "C", "Java", "Linux", "SQL"};
    job.minimum_years_experience = 1;
    job.salary = "25 LPA";

    const auto result = scorer.score(job);

    EXPECT_GE(result.score, 0);
    EXPECT_LE(result.score, 100);
}

} // namespace
} // namespace orbit
