#include "PostgresConnection.h"

#include <stdexcept>

#if defined(__has_include)
#  if __has_include(<libpq-fe.h>)
#    include <libpq-fe.h>
#  elif __has_include(<postgresql/libpq-fe.h>)
#    include <postgresql/libpq-fe.h>
#  else
#    error "libpq development headers not found"
#  endif
#else
#  include <libpq-fe.h>
#endif

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

if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
                const std::string error =
            PQerrorMessage(impl_->connection);

        PQclear(result);

        throw std::runtime_error(
            "PostgreSQL query failed: " + error
        );
    }

    PQclear(result);
}

} // namespace orbit