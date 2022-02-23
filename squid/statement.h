//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "api.h"
#include "basicstatement.h"

#include <string_view>

namespace squid {

class Connection;

class SQUID_API Statement final : public BasicStatement
{
public:
	/// Create a simple statement defined by @a query on @a connection.
	/// This statement can be executed multiple times, but it is not
	/// intended as a prepared statement.
	explicit Statement(Connection& connection, std::string_view query);

	using BasicStatement::bind;
	using BasicStatement::bindResult;
	using BasicStatement::execute;
	using BasicStatement::fetch;
};

} // namespace squid