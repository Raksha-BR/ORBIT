#include <gtest/gtest.h>

#include "candidate/CandidateProfileLoader.h"
#include "storage/InMemoryCandidateFactRepository.h"

namespace orbit {

TEST(CandidateProfileLoaderTest, LoadsVerifiedAndDerivedFacts) {
    InMemoryCandidateFactRepository repository;

    repository.save({
        .key = "profile.name",
        .value = "Raksha",
        .confidence = FactConfidence::Verified
    });

    repository.save({
        .key = "skill",
        .value = "C++",
        .confidence = FactConfidence::Verified
    });

    repository.save({
        .key = "skill",
        .value = "Python",
        .confidence = FactConfidence::Derived
    });

    repository.save({
        .key = "experience.years",
        .value = "1",
        .confidence = FactConfidence::Verified
    });

    const Candidate candidate =
        CandidateProfileLoader{}.load(repository);

    EXPECT_EQ(candidate.name, "Raksha");
    ASSERT_EQ(candidate.skills.size(), 2);
    EXPECT_EQ(candidate.years_of_experience, 1);
}

TEST(CandidateProfileLoaderTest, IgnoresUncertainFacts) {
    InMemoryCandidateFactRepository repository;

    repository.save({
        .key = "skill",
        .value = "Rust",
        .confidence = FactConfidence::Uncertain
    });

    const Candidate candidate =
        CandidateProfileLoader{}.load(repository);

    EXPECT_TRUE(candidate.skills.empty());
}

TEST(CandidateProfileLoaderTest, HandlesMissingExperience) {
    InMemoryCandidateFactRepository repository;

    const Candidate candidate =
        CandidateProfileLoader{}.load(repository);

    EXPECT_FALSE(candidate.years_of_experience.has_value());
}

TEST(CandidateProfileLoaderTest, IgnoresInvalidExperience) {
    InMemoryCandidateFactRepository repository;

    repository.save({
        .key = "experience.years",
        .value = "unknown",
        .confidence = FactConfidence::Verified
    });

    const Candidate candidate =
        CandidateProfileLoader{}.load(repository);

    EXPECT_FALSE(candidate.years_of_experience.has_value());
}

TEST(CandidateProfileLoaderTest, LoadsPreferences) {
    InMemoryCandidateFactRepository repository;

    repository.save({
        .key = "preference.role",
        .value = "SDE",
        .confidence = FactConfidence::Verified
    });

    repository.save({
        .key = "preference.location",
        .value = "Bengaluru",
        .confidence = FactConfidence::Verified
    });

    repository.save({
        .key = "preference.expected_salary",
        .value = "1500000",
        .confidence = FactConfidence::Verified
    });

    const Candidate candidate =
        CandidateProfileLoader{}.load(repository);

    ASSERT_EQ(candidate.preferred_roles.size(), 1);
    EXPECT_EQ(candidate.preferred_roles[0], "SDE");

    ASSERT_EQ(candidate.preferred_locations.size(), 1);
    EXPECT_EQ(candidate.preferred_locations[0], "Bengaluru");

    EXPECT_EQ(candidate.expected_salary, 1500000);
}

} // namespace orbit