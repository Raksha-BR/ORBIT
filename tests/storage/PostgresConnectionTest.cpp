#include <gtest/gtest.h>

#include "storage/postgres/PostgresConnection.h"

namespace orbit {

TEST(PostgresConnectionTest, ConnectsToDatabase) {
    ASSERT_NO_THROW({
        PostgresConnection connection(
            "host=localhost "
            "port=5432 "
            "dbname=orbit "
            "user=orbit_user "
            "password=orbit_dev_password"
        );

        connection.execute(
            "SELECT 1"
        );
    });
}

} // namespace orbit