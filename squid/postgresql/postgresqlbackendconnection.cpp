//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "postgresqlbackendconnection.h"
#include "postgresqlstatement.h"
#include "postgresqlpreparedstatement.h"
#include "postgresqlerror.h"

namespace squid {
namespace postgresql {

std::unique_ptr<IBackendStatement> PostgresqlBackendConnection::createStatement(std::string_view query)
{
	return std::make_unique<PostgresqlStatement>(this->connection_, query);
}

std::unique_ptr<IBackendStatement> PostgresqlBackendConnection::createPreparedStatement(std::string_view query)
{
	return std::make_unique<PostgresqlPreparedStatement>(this->connection_, query);
}

PostgresqlBackendConnection::PostgresqlBackendConnection(const std::string& connectionInfo)
    : connection_{ PQconnectdb(connectionInfo.c_str()), PQfinish }
{
	if (this->connection_)
	{
		if (CONNECTION_OK != PQstatus(this->connection_.get()))
		{
			throw PostgresqlError{ "PQconnectdb failed", *this->connection_.get() };
		}
	}
	else
	{
		throw PostgresqlError{ "PQconnectdb failed" };
	}
}

PGconn& PostgresqlBackendConnection::handle() const
{
	return *this->connection_;
}

} // namespace postgresql
} // namespace squid
