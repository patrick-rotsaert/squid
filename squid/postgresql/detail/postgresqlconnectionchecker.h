//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <memory>

#include <libpq-fe.h>

namespace squid {
namespace postgresql {

class PostgresqlConnectionChecker final
{
public:
	// must not return a nullptr
	static PGconn* check(PGconn* connection);

	// must not return a nullptr
	// connection must outlive the returned pointer
	static PGconn* check(std::shared_ptr<PGconn> connection);
};

} // namespace postgresql
} // namespace squid
