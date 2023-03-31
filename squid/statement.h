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

/// This statement class is intended for one-off operations.
/// Some backends, e.g. SQLite3, implement this with a prepared statement
/// but others, e.g. PostgreSQL do not.
/// So, even though this statement can be executed multiple times, note that
/// the backend may need to plan the query each time.
/// For bulk operations, the prepared_statement class is better suited.
class SQUID_EXPORT statement final : public basic_statement
{
	std::unique_ptr<ibackend_statement> create_statement(std::shared_ptr<ibackend_connection> connection, std::string_view query) override;

public:
	/// Create a simple statement defined by @a query on @a connection.
	explicit statement(connection& connection, std::string_view query);

	/// Create a simple statement on @a connection, without a query.
	/// The query must be provided later on with the methods query() or operator<<.
	explicit statement(connection& connection);

	using basic_statement::operator<<;
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
