//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/sqlite3/error.h"

#include <sstream>

#include <sqlite3.h>

namespace squid {
namespace sqlite {

namespace {

std::string build_message(const std::string& message, sqlite3& connection)
{
	return message + ": " + sqlite3_errmsg(&connection);
}

std::string build_message(const std::string& message, int errorCode)
{
	return message + ": " + sqlite3_errstr(errorCode);
}

} // namespace

error::error(const std::string& message)
    : squid::error{ message }
    , ec_{}
{
}

error::error(const std::string& message, sqlite3& connection)
    : squid::error{ build_message(message, connection) }
    , ec_{ sqlite3_errcode(&connection) }
{
}

error::error(const std::string& message, int ec)
    : squid::error{ build_message(message, ec) }
    , ec_{ ec }
{
}

const std::optional<int>& error::ec() const
{
	return this->ec_;
}

} // namespace sqlite
} // namespace squid
