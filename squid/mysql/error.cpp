//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/mysql/error.h"

#include <sstream>

#include <mysql/mysql.h>

namespace squid {
namespace mysql {

namespace {

std::string build_message(const std::string& message, MYSQL& connection)
{
	return message + ": " + mysql_error(&connection);
}

std::string build_message(const std::string& message, MYSQL_STMT& stmt)
{
	return message + ": " + mysql_stmt_error(&stmt);
}

} // namespace

error::error(const std::string& message)
    : squid::error{ message }
    , ec_{}
{
}

error::error(const std::string& message, MYSQL& connection)
    : squid::error{ build_message(message, connection) }
    , ec_{ mysql_errno(&connection) }
{
}

error::error(const std::string& message, MYSQL_STMT& stmt)
    : squid::error{ build_message(message, stmt) }
    , ec_{ mysql_stmt_errno(&stmt) }
{
}

const std::optional<unsigned int>& error::ec() const
{
	return this->ec_;
}

} // namespace mysql
} // namespace squid
