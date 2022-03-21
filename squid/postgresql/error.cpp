//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/postgresql/error.h"

#include <sstream>

#include <libpq-fe.h>

namespace squid {
namespace postgresql {

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

Error::Error(const std::string& message)
    : squid::Error{ message }
    , sqlState_()
{
}

Error::Error(const std::string& message, const PGconn& connection)
    : squid::Error{ build_message(message, connection) }
    , sqlState_()
{
}

Error::Error(const std::string& message, const PGconn& connection, const PGresult& result)
    : squid::Error{ build_message(message, connection, result) }
    , sqlState_()
{
	auto sqlState = PQresultErrorField(&result, PG_DIAG_SQLSTATE);
	if (sqlState)
	{
		this->sqlState_ = std::string{ sqlState, 5 };
	}
}

const std::optional<std::string>& Error::sqlState() const
{
	return this->sqlState_;
}

} // namespace postgresql
} // namespace squid
