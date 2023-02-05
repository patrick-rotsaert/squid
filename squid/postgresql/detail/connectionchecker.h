//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/postgresql/detail/libpqfwd.h"

#include <memory>

namespace squid {
namespace postgresql {

class ConnectionChecker final
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
