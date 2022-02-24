//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/postgresql/backendconnection.h"
#include "squid/postgresql/statement.h"
#include "squid/postgresql/preparedstatement.h"
#include "squid/postgresql/error.h"

#include "squid/postgresql/detail/connectionchecker.h"

namespace squid {
namespace postgresql {

std::unique_ptr<IBackendStatement> BackendConnection::createStatement(std::string_view query)
{
	return std::make_unique<Statement>(this->connection_, query);
}

std::unique_ptr<IBackendStatement> BackendConnection::createPreparedStatement(std::string_view query)
{
	return std::make_unique<PreparedStatement>(this->connection_, query);
}

void BackendConnection::execute(const std::string& query)
{
	std::shared_ptr<PGresult> result{ PQexec(ConnectionChecker::check(this->connection_), query.c_str()), PQclear };
	if (result)
	{
		const auto status = PQresultStatus(result.get());
		if (PGRES_TUPLES_OK != status && PGRES_COMMAND_OK != status)
		{
			throw Error{ "PQexec failed", *this->connection_, *result.get() };
		}
	}
	else
	{
		throw Error{ "PQexec failed", *this->connection_ };
	}
}

BackendConnection::BackendConnection(const std::string& connectionInfo)
    : connection_{ PQconnectdb(connectionInfo.c_str()), PQfinish }
{
	if (this->connection_)
	{
		if (CONNECTION_OK != PQstatus(this->connection_.get()))
		{
			throw Error{ "PQconnectdb failed", *this->connection_.get() };
		}
	}
	else
	{
		throw Error{ "PQconnectdb failed" };
	}
}

PGconn& BackendConnection::handle() const
{
	return *this->connection_;
}

} // namespace postgresql
} // namespace squid
