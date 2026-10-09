#include "PostgresCandidateFactRepository.h"

#include <string>

namespace orbit {

namespace {

std::string confidence_to_string(
    FactConfidence confidence
) {
    switch (confidence) {

        case FactConfidence::Verified:
            return "verified";

        case FactConfidence::Derived:
            return "derived";

        case FactConfidence::Uncertain:
            return "uncertain";
    }

    return "uncertain";
}

} // namespace

PostgresCandidateFactRepository::
    PostgresCandidateFactRepository(
        PostgresConnection& connection
    )
    : connection_(connection) {
}

void PostgresCandidateFactRepository::save(
    const CandidateFact& fact
) {
    connection_.execute(
        R"SQL(
            INSERT INTO candidate_facts (
                fact_key,
                fact_value,
                confidence
            )
            VALUES ($1, $2, $3)
        )SQL",
        {
            fact.key,
            fact.value,
            confidence_to_string(fact.confidence)
        }
    );
}

std::optional<CandidateFact>
PostgresCandidateFactRepository::find(
    const std::string& key
) const {
    const auto rows = connection_.query(
        R"SQL(
            SELECT
                fact_key,
                fact_value,
                confidence
            FROM candidate_facts
            WHERE fact_key = $1
            ORDER BY id
            LIMIT 1
        )SQL",
        {key}
    );

    if (rows.empty()) {
        return std::nullopt;
    }

    CandidateFact fact;

    fact.key = rows[0][0];
    fact.value = rows[0][1];

    if (rows[0][2] == "verified") {
        fact.confidence = FactConfidence::Verified;
    } else if (rows[0][2] == "derived") {
        fact.confidence = FactConfidence::Derived;
    } else {
        fact.confidence = FactConfidence::Uncertain;
    }

    return fact;
}

} // namespace orbit