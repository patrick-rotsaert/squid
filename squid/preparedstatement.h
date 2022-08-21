//
// Copyright (C) 2022 Patrick Rotsaert
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

class SQUID_EXPORT PreparedStatement final : public BasicStatement
{
public:
	/// Create a prepared statement defined by @a query on @a connection.
	/// This statement is intended for bulk operations, i.e. prepare the statement
	/// once and execute it many times with different parameter bindings.
	explicit PreparedStatement(Connection& connection, std::string_view query);

	using BasicStatement::bind;
	using BasicStatement::bindResult;
	using BasicStatement::bindResults;
	using BasicStatement::execute;
	using BasicStatement::fetch;
};

} // namespace squid
