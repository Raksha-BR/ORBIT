#pragma once

#include "Answer.h"
#include "ApplicationQuestion.h"

#include "../candidate/CandidateFactStore.h"

namespace orbit {

class AnswerEngine {
public:
    explicit AnswerEngine(
        const CandidateFactStore& fact_store
    );

    Answer answer(
        const ApplicationQuestion& question
    ) const;

private:
    const CandidateFactStore& fact_store_;
};

} // namespace orbit