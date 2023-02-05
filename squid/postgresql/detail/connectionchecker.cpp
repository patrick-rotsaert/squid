//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/postgresql/detail/connectionchecker.h"

#include "squid/postgresql/error.h"

#include <cassert>

#include <libpq-fe.h>

namespace squid {
namespace postgresql {

PGconn* ConnectionChecker::check(PGconn* connection)
{
	assert(connection);
	if (CONNECTION_OK != PQstatus(connection))
	{
		PQreset(connection);
		if (CONNECTION_OK != PQstatus(connection))
		{
			throw Error{ "PQreset failed", *connection };
		}
	}
	return connection;
}

PGconn* ConnectionChecker::check(std::shared_ptr<PGconn> connection)
{
	return ConnectionChecker::check(connection.get());
}

} // namespace postgresql
} // namespace squid
