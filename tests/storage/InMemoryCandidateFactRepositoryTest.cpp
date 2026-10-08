#include <gtest/gtest.h>

#include "storage/InMemoryCandidateFactRepository.h"

namespace orbit {

TEST(
    InMemoryCandidateFactRepositoryTest,
    SavesAndFindsFact
) {
    InMemoryCandidateFactRepository repository;

    CandidateFact fact{
        .key = "skill",
        .value = "C++",
        .confidence = FactConfidence::Verified
    };

    repository.save(fact);

    const auto result =
        repository.find("skill");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(
        result->value,
        "C++"
    );
}

TEST(
    InMemoryCandidateFactRepositoryTest,
    ReturnsEmptyForUnknownFact
) {
    InMemoryCandidateFactRepository repository;

    const auto result =
        repository.find("unknown");

    EXPECT_FALSE(result.has_value());
}

TEST(
    InMemoryCandidateFactRepositoryTest,
    ReturnsAllFacts
) {
    InMemoryCandidateFactRepository repository;

    repository.save(
        CandidateFact{
            .key = "skill",
            .value = "C++",
            .confidence = FactConfidence::Verified
        }
    );

    repository.save(
        CandidateFact{
            .key = "skill",
            .value = "Python",
            .confidence = FactConfidence::Verified
        }
    );

    const auto facts =
        repository.find_all();

    EXPECT_EQ(facts.size(), 2);
}

} // namespace orbit