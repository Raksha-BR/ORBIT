#include <gtest/gtest.h>

#include "applications/AnswerEngine.h"

namespace orbit {

TEST(AnswerEngineTest, AnswersPreviousEmploymentFromVerifiedFact) {

    CandidateFactStore store;

    store.add_fact(
        CandidateFact{
            .key = "employment.company",
            .value = "Dell Technologies",
            .confidence = FactConfidence::Verified
        }
    );

    AnswerEngine engine(store);

    ApplicationQuestion question{
        .text = "Have you previously worked at Dell Technologies?"
    };

    const auto answer = engine.answer(question);

    EXPECT_EQ(
        answer.value,
        "YES"
    );

    EXPECT_EQ(
        answer.classification,
        AnswerClassification::AutoAnswer
    );
}

TEST(AnswerEngineTest, DoesNotGuessSensitiveInformation) {

    CandidateFactStore store;

    AnswerEngine engine(store);

    ApplicationQuestion question{
        .text = "Are you a veteran?"
    };

    const auto answer = engine.answer(question);

    EXPECT_EQ(
        answer.classification,
        AnswerClassification::UserOnly
    );

    EXPECT_TRUE(
        answer.value.empty()
    );
}

TEST(AnswerEngineTest, UnknownQuestionIsNotGuessed) {

    CandidateFactStore store;

    AnswerEngine engine(store);

    ApplicationQuestion question{
        .text = "Why do you want to work here?"
    };

    const auto answer = engine.answer(question);

    EXPECT_EQ(
        answer.classification,
        AnswerClassification::Unknown
    );

    EXPECT_TRUE(
        answer.value.empty()
    );
}

} // namespace orbit