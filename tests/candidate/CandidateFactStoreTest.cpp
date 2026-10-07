#include <gtest/gtest.h>

#include "candidate/CandidateFactStore.h"

namespace orbit {

TEST(CandidateFactStoreTest, StoresAndFindsFact) {
    CandidateFactStore store;

    CandidateFact fact{
        .key = "employment.company",
        .value = "Dell Technologies",
        .confidence = FactConfidence::Verified,
        .evidence = {
            Evidence{
                .source = EvidenceSource::Resume,
                .reference = "resume_v1",
                .excerpt =
                    "Software Engineer II at Dell Technologies"
            }
        }
    };

    store.add_fact(fact);

    const auto result =
        store.find_fact("employment.company");

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(
        result->value,
        "Dell Technologies"
    );

    EXPECT_EQ(
        result->confidence,
        FactConfidence::Verified
    );
}

TEST(CandidateFactStoreTest, ReturnsNothingForUnknownFact) {
    CandidateFactStore store;

    EXPECT_FALSE(
        store.find_fact("employment.company").has_value()
    );
}

TEST(CandidateFactStoreTest, ReportsExistingFact) {
    CandidateFactStore store;

    CandidateFact fact{
        .key = "skill",
        .value = "C++",
        .confidence = FactConfidence::Verified
    };

    store.add_fact(fact);

    EXPECT_TRUE(
        store.has_fact("skill")
    );
}

} // namespace orbit