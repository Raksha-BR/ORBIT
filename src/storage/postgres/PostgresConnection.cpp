#include "PostgresConnection.h"

#include <stdexcept>
#include <utility>
#include <vector>

#include <libpq-fe.h>

namespace orbit {

struct PostgresConnection::Impl {
    PGconn* connection{nullptr};
};

PostgresConnection::PostgresConnection(
    const std::string& connection_string
)
    : impl_(std::make_unique<Impl>()) {

    impl_->connection =
        PQconnectdb(connection_string.c_str());

    if (PQstatus(impl_->connection) != CONNECTION_OK) {
        const std::string error =
            PQerrorMessage(impl_->connection);

        PQfinish(impl_->connection);
        impl_->connection = nullptr;

        throw std::runtime_error(
            "PostgreSQL connection failed: " + error
        );
    }
}

PostgresConnection::~PostgresConnection() {
    if (impl_ && impl_->connection) {
        PQfinish(impl_->connection);
    }
}

void PostgresConnection::execute(
    const std::string& sql
) {
    PGresult* result =
        PQexec(impl_->connection, sql.c_str());

    const auto status = PQresultStatus(result);

    if (status != PGRES_COMMAND_OK &&
        status != PGRES_TUPLES_OK) {

        const std::string error =
            PQerrorMessage(impl_->connection);

        PQclear(result);

        throw std::runtime_error(
            "PostgreSQL query failed: " + error
        );
    }

    PQclear(result);
}

void PostgresConnection::execute(
    const std::string& sql,
    const std::vector<std::string>& parameters
) {
    std::vector<const char*> values;
    values.reserve(parameters.size());

    for (const auto& parameter : parameters) {
        values.push_back(parameter.c_str());
    }

    PGresult* result =
        PQexecParams(
            impl_->connection,
            sql.c_str(),
            static_cast<int>(values.size()),
            nullptr,
            values.data(),
            nullptr,
            nullptr,
            0
        );

    const auto status = PQresultStatus(result);

    if (status != PGRES_COMMAND_OK &&
        status != PGRES_TUPLES_OK) {

        const std::string error =
            PQerrorMessage(impl_->connection);

        PQclear(result);

        throw std::runtime_error(
            "PostgreSQL query failed: " + error
        );
    }

    PQclear(result);
}

std::vector<std::vector<std::string>>
PostgresConnection::query(
    const std::string& sql,
    const std::vector<std::string>& parameters
) const {
    std::vector<const char*> values;
    values.reserve(parameters.size());

    for (const auto& parameter : parameters) {
        values.push_back(parameter.c_str());
    }

    PGresult* result =
        PQexecParams(
            impl_->connection,
            sql.c_str(),
            static_cast<int>(values.size()),
            nullptr,
            values.data(),
            nullptr,
            nullptr,
            0
        );

    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        const std::string error =
            PQerrorMessage(impl_->connection);

        PQclear(result);

        throw std::runtime_error(
            "PostgreSQL query failed: " + error
        );
    }

    std::vector<std::vector<std::string>> rows;

    const int row_count = PQntuples(result);
    const int column_count = PQnfields(result);

    for (int row = 0; row < row_count; ++row) {
        std::vector<std::string> row_values;

        for (int column = 0; column < column_count; ++column) {
            if (PQgetisnull(result, row, column)) {
                row_values.emplace_back();
            } else {
                row_values.emplace_back(
                    PQgetvalue(result, row, column)
                );
            }
        }

        rows.push_back(std::move(row_values));
    }

    PQclear(result);

    return rows;
}

} // namespace orbit