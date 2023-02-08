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

class connection;

class SQUID_EXPORT prepared_statement final : public basic_statement
{
public:
	/// Create a prepared statement defined by @a query on @a connection.
	/// This statement is intended for bulk operations, i.e. prepare the statement
	/// once and execute it many times with different parameter bindings.
	explicit prepared_statement(connection& connection, std::string_view query);

	using basic_statement::bind;
	using basic_statement::bind_ref;
	using basic_statement::bind_result;
	using basic_statement::bind_results;
	using basic_statement::execute;
	using basic_statement::fetch;
	using basic_statement::field_count;
	using basic_statement::field_name;
};

} // namespace squid
