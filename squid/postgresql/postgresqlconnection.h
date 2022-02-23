//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/connection.h"
#include "squid/api.h"

#include "postgresqlbackendconnection.h"

namespace squid {
namespace postgresql {

// Convenience class to create a connection to a PostgreSQL backend
// This class should be used if access to the native connection handle (PGconn) is needed.
class SQUID_API PostgresqlConnection final : public Connection
{
	std::shared_ptr<PostgresqlBackendConnection> backend_;

public:
	explicit PostgresqlConnection(std::string_view connectionInfo);

	PostgresqlConnection(const PostgresqlConnection&) = delete;
	PostgresqlConnection(PostgresqlConnection&& src)  = default;
	PostgresqlConnection& operator=(const PostgresqlConnection&) = delete;
	PostgresqlConnection& operator=(PostgresqlConnection&&) = default;

	const PostgresqlBackendConnection& backendConnection() const;
};

} // namespace postgresql
} // namespace squid
