//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "postgresqlerror.h"

#include <sstream>

namespace squid {

namespace {

std::string build_message(const std::string& message, const PGconn& connection)
{
	std::ostringstream error;
	error << message;

	auto pqMessage = PQerrorMessage(&connection);
	if (pqMessage)
	{
		error << "\n" << pqMessage;
	}

	return error.str();
}

std::string build_message(const std::string& message, const PGconn& connection, const PGresult& result)
{
	std::ostringstream error;
	error << message;

	auto statusName = PQresStatus(PQresultStatus(&result));
	if (statusName)
	{
		error << " (" << statusName << ")";
	}

	auto pqMessage = PQresultErrorMessage(&result);
	if (!pqMessage)
	{
		pqMessage = PQerrorMessage(&connection);
	}

	if (pqMessage)
	{
		error << "\n" << pqMessage;
	}

	return error.str();
}

} // namespace

PostgresqlError::PostgresqlError(const std::string& message)
    : Error{ message }
    , sqlState_()
{
}

PostgresqlError::PostgresqlError(const std::string& message, const PGconn& connection)
    : Error{ build_message(message, connection) }
    , sqlState_()
{
}

PostgresqlError::PostgresqlError(const std::string& message, const PGconn& connection, const PGresult& result)
    : Error{ build_message(message, connection, result) }
    , sqlState_()
{
	auto sqlState = PQresultErrorField(&result, PG_DIAG_SQLSTATE);
	if (sqlState)
	{
		this->sqlState_ = std::string{ sqlState, 5 };
	}
}

const std::optional<std::string>& PostgresqlError::sqlState() const
{
	return this->sqlState_;
}

} // namespace squid
