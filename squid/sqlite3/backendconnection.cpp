//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/sqlite3/backendconnection.h"
#include "squid/sqlite3/statement.h"
#include "squid/sqlite3/error.h"

#include <sqlite3.h>

namespace squid {
namespace sqlite {

namespace {

sqlite3* connect_database(const std::string& connectionInfo)
{
	sqlite3* handle{ nullptr };
	auto     err = sqlite3_open(connectionInfo.c_str(), &handle);
	if (SQLITE_OK != err)
	{
		throw Error{ "sqlite3_open failed", err };
	}
	else if (!handle)
	{
		throw Error{ "sqlite3_open did not set the connection handle" };
	}
	else
	{
		return handle;
	}
}

} // namespace

// In SQLite there is no distinction between regular statements and prepared statements.
// All statements are prepared statements.

std::unique_ptr<IBackendStatement> BackendConnection::createStatement(std::string_view query)
{
	return std::make_unique<Statement>(this->connection_, query, false);
}

std::unique_ptr<IBackendStatement> BackendConnection::createPreparedStatement(std::string_view query)
{
	return std::make_unique<Statement>(this->connection_, query, true);
}

void BackendConnection::execute(const std::string& query)
{
	Statement::execute(*this->connection_, query);
}

BackendConnection::BackendConnection(const std::string& connectionInfo)
    : connection_{ connect_database(connectionInfo), sqlite3_close }
{
}

sqlite3& BackendConnection::handle() const
{
	return *this->connection_;
}

} // namespace sqlite
} // namespace squid
