#include "AnswerEngine.h"

#include <algorithm>
#include <cctype>

namespace orbit {

namespace {

std::string normalize(std::string value) {
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );

    return value;
}

bool contains(
    const std::string& text,
    const std::string& phrase
) {
    return normalize(text).find(normalize(phrase))
        != std::string::npos;
}

} // namespace

AnswerEngine::AnswerEngine(
    const CandidateFactStore& fact_store
)
    : fact_store_(fact_store) {
}

Answer AnswerEngine::answer(
    const ApplicationQuestion& question
) const {

    const auto& text = question.text;

    // --------------------------------------------------------
    // Sensitive questions
    // --------------------------------------------------------

    if (contains(text, "veteran") ||
        contains(text, "military service") ||
        contains(text, "disability") ||
        contains(text, "disabled")) {

        return {
            .value = "",
            .classification =
                AnswerClassification::UserOnly,
            .reason =
                "Sensitive information must be explicitly "
                "provided and controlled by the user."
        };
    }

    // --------------------------------------------------------
    // Previous employment
    // --------------------------------------------------------

    if (contains(text, "worked at") ||
        contains(text, "previously worked") ||
        contains(text, "previous employee")) {

        if (const auto fact =
                fact_store_.find_fact("employment.company");
            fact.has_value()) {

            return {
                .value = "YES",
                .classification =
                    AnswerClassification::AutoAnswer,
                .reason =
                    "Verified employment fact: " +
                    fact->value
            };
        }

        return {
            .value = "",
            .classification =
                AnswerClassification::Unknown,
            .reason =
                "No verified employment history was found."
        };
    }

    return {
        .value = "",
        .classification =
            AnswerClassification::Unknown,
        .reason =
            "ORBIT does not have enough information to "
            "answer this question safely."
    };
}

} // namespace orbit