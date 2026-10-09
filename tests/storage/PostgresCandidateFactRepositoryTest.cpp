#include <gtest/gtest.h>

#include "storage/postgres/PostgresCandidateFactRepository.h"
#include "storage/postgres/PostgresConnection.h"
#include "storage/postgres/PostgresSchema.h"

namespace orbit {

namespace {

PostgresConnection create_connection() {
    return PostgresConnection(
        "host=localhost "
        "port=5432 "
        "dbname=orbit "
        "user=orbit_user "
        "password=orbit_dev_password"
    );
}

} // namespace

TEST(
    PostgresCandidateFactRepositoryTest,
    SavesAndFindsFact
) {
    auto connection = create_connection();
    PostgresSchema::initialize(connection);

    connection.execute("DELETE FROM candidate_facts");

    PostgresCandidateFactRepository repository(connection);

    CandidateFact fact{
        .key = "skill",
        .value = "C++",
        .confidence = FactConfidence::Verified
    };

    repository.save(fact);

    const auto result = repository.find("skill");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->key, "skill");
    EXPECT_EQ(result->value, "C++");
    EXPECT_EQ(result->confidence, FactConfidence::Verified);
}

TEST(
    PostgresCandidateFactRepositoryTest,
    ReturnsNothingForUnknownFact
) {
    auto connection = create_connection();
    PostgresSchema::initialize(connection);

    connection.execute("DELETE FROM candidate_facts");

    PostgresCandidateFactRepository repository(connection);

    EXPECT_FALSE(
        repository.find("does.not.exist").has_value()
    );
}

TEST(
    PostgresCandidateFactRepositoryTest,
    ReturnsAllFacts
) {
    auto connection = create_connection();
    PostgresSchema::initialize(connection);

    connection.execute("DELETE FROM candidate_facts");

    PostgresCandidateFactRepository repository(connection);

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
            .confidence = FactConfidence::Derived
        }
    );

    const auto facts = repository.find_all();

    ASSERT_EQ(facts.size(), 2);
    EXPECT_EQ(facts[0].value, "C++");
    EXPECT_EQ(facts[1].value, "Python");
    EXPECT_EQ(facts[1].confidence, FactConfidence::Derived);
}

} // namespace orbit