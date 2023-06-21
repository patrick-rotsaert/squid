//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/postgresql/backendconnection.h"
#include "squid/postgresql/statement.h"
#include "squid/postgresql/error.h"

#include "squid/postgresql/detail/connectionchecker.h"

#include <libpq-fe.h>

namespace squid {
namespace postgresql {

std::unique_ptr<ibackend_statement> backend_connection::create_statement(std::string_view query)
{
	return std::make_unique<statement>(this->connection_, query, false);
}

std::unique_ptr<ibackend_statement> backend_connection::create_prepared_statement(std::string_view query)
{
	return std::make_unique<statement>(this->connection_, query, true);
}

/* static */ void backend_connection::execute(const std::string& query)
{
	statement::execute(*connection_checker::check(this->connection_), query);
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
