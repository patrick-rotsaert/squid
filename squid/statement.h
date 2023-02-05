//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/basicstatement.h"

#include <string_view>

namespace squid {

class Connection;

class SQUID_EXPORT Statement final : public BasicStatement
{
public:
	/// Create a simple statement defined by @a query on @a connection.
	/// Some backends, e.g. SQLite3, implement this with a prepared statement
	/// but others, e.g. PostgreSQL do not.
	/// So, even though this statement can be executed multiple times, note that
	/// the backend may need to plan the query each time.
	/// This class is should only be used for one-off statements.
	/// For bulk operations, the PreparedStatement class is better suited.
	explicit Statement(Connection& connection, std::string_view query);

	using BasicStatement::bind;
	using BasicStatement::bindResult;
	using BasicStatement::bindResults;
	using BasicStatement::execute;
	using BasicStatement::fetch;
};

} // namespace squid
