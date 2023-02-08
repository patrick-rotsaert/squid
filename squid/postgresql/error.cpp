//
// Copyright (C) 2022-2023 Patrick Rotsaert
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
	std::ostringstream msg;
	msg << message;

	auto pqmessage = PQerrorMessage(&connection);
	if (pqmessage)
	{
		msg << "\n" << pqmessage;
	}

	return msg.str();
}

std::string build_message(const std::string& message, const PGconn& connection, const PGresult& result)
{
	std::ostringstream msg;
	msg << message;

	auto status_name = PQresStatus(PQresultStatus(&result));
	if (status_name)
	{
		msg << " (" << status_name << ")";
	}

	auto pqmessage = PQresultErrorMessage(&result);
	if (!pqmessage)
	{
		pqmessage = PQerrorMessage(&connection);
	}

	if (pqmessage)
	{
		msg << "\n" << pqmessage;
	}

	return msg.str();
}

} // namespace

error::error(const std::string& message)
    : squid::error{ message }
    , sql_state_()
{
}

error::error(const std::string& message, const PGconn& connection)
    : squid::error{ build_message(message, connection) }
    , sql_state_()
{
}

error::error(const std::string& message, const PGconn& connection, const PGresult& result)
    : squid::error{ build_message(message, connection, result) }
    , sql_state_()
{
	auto sql_state = PQresultErrorField(&result, PG_DIAG_SQLSTATE);
	if (sql_state)
	{
		this->sql_state_ = std::string{ sql_state, 5 };
	}
}

const std::optional<std::string>& error::sql_state() const
{
	return this->sql_state_;
}

} // namespace postgresql
} // namespace squid
