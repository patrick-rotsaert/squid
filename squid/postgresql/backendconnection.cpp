//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/postgresql/backendconnection.h"
#include "squid/postgresql/statement.h"
#include "squid/postgresql/preparedstatement.h"
#include "squid/postgresql/error.h"

#include "squid/postgresql/detail/connectionchecker.h"

#include <libpq-fe.h>

namespace squid {
namespace postgresql {

std::unique_ptr<ibackend_statement> backend_connection::create_statement(std::string_view query)
{
	return std::make_unique<statement>(this->connection_, query);
}

std::unique_ptr<ibackend_statement> backend_connection::create_prepared_statement(std::string_view query)
{
	return std::make_unique<prepared_statement>(this->connection_, query);
}

void backend_connection::execute(const std::string& query)
{
	std::shared_ptr<PGresult> result{ PQexec(connection_checker::check(this->connection_), query.c_str()), PQclear };
	if (result)
	{
		const auto status = PQresultStatus(result.get());
		if (PGRES_TUPLES_OK != status && PGRES_COMMAND_OK != status)
		{
			throw error{ "PQexec failed", *this->connection_, *result.get() };
		}
	}
	else
	{
		throw error{ "PQexec failed", *this->connection_ };
	}
}

backend_connection::backend_connection(const std::string& connection_info)
    : connection_{ PQconnectdb(connection_info.c_str()), PQfinish }
{
	if (this->connection_)
	{
		if (CONNECTION_OK != PQstatus(this->connection_.get()))
		{
			throw error{ "PQconnectdb failed", *this->connection_.get() };
		}
	}
	else
	{
		throw error{ "PQconnectdb failed" };
	}
}

PGconn& backend_connection::handle() const
{
	return *this->connection_;
}

} // namespace postgresql
} // namespace squid
