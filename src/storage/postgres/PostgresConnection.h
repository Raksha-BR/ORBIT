#pragma once

#include <memory>
#include <string>

namespace orbit {

class PostgresConnection {
public:
    explicit PostgresConnection(
        const std::string& connection_string
    );

    ~PostgresConnection();

    PostgresConnection(
        const PostgresConnection&
    ) = delete;

    PostgresConnection& operator=(
        const PostgresConnection&
    ) = delete;

    void execute(
        const std::string& sql
    );

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace orbit