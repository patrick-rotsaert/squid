//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "postgresqlconnectionchecker.h"

#include "squid/postgresql/postgresqlerror.h"

#include <cassert>

namespace squid {

PGconn* PostgresqlConnectionChecker::check(PGconn* connection)
{
	assert(connection);
	if (CONNECTION_OK != PQstatus(connection))
	{
		PQreset(connection);
		if (CONNECTION_OK != PQstatus(connection))
		{
			throw PostgresqlError{ "PQreset failed", *connection };
		}
	}
	return connection;
}

PGconn* PostgresqlConnectionChecker::check(std::shared_ptr<PGconn> connection)
{
	return PostgresqlConnectionChecker::check(connection.get());
}

} // namespace squid
