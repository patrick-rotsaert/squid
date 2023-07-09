//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/sqlite3/backendconnection.h"
#include "squid/sqlite3/statement.h"
#include "squid/sqlite3/error.h"

#include "squid/sqlite3/detail/isqliteapi.h"

#include <sqlite3.h>

namespace squid {
namespace sqlite {

namespace {

sqlite3* connect_database(isqlite_api& api, const std::string& connection_info)
{
	sqlite3* handle{};
	auto     err = api.open(connection_info.c_str(), &handle);
	if (SQLITE_OK != err)
	{
		throw error{ api, "sqlite3_open failed", err };
	}
	else if (!handle)
	{
		throw error{ "sqlite3_open did not set the connection handle" };
	}
	else
	{
		return handle;
	}
}

} // namespace

// In SQLite there is no distinction between regular statements and prepared statements.
// All statements are prepared statements.

std::unique_ptr<ibackend_statement> backend_connection::create_statement(std::string_view query)
{
	return std::make_unique<statement>(*this->api_, this->connection_, query, false);
}

std::unique_ptr<ibackend_statement> backend_connection::create_prepared_statement(std::string_view query)
{
	return std::make_unique<statement>(*this->api_, this->connection_, query, true);
}

void backend_connection::execute(const std::string& query)
{
	statement::execute(*this->api_, *this->connection_, query);
}

backend_connection::backend_connection(isqlite_api& api, const std::string& connection_info)
    : api_{ &api }
    , connection_{ connect_database(api, connection_info), [&api](sqlite3* db) { api.close(db); } }
{
}

sqlite3& backend_connection::handle() const
{
	return *this->connection_;
}

} // namespace sqlite
} // namespace squid
