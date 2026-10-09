#pragma once

#include <memory>
#include <string>
#include <vector>

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

    void execute(
        const std::string& sql,
        const std::vector<std::string>& parameters
    );

    std::vector<std::vector<std::string>> query(
        const std::string& sql,
        const std::vector<std::string>& parameters = {}
    ) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace orbit