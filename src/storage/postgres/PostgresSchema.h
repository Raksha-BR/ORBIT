#pragma once

namespace orbit {

class PostgresConnection;

class PostgresSchema {
public:
    static void initialize(
        PostgresConnection& connection
    );
};

} // namespace orbit