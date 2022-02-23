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
