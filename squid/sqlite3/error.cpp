//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/sqlite3/error.h"
#include "squid/sqlite3/detail/isqliteapi.h"

#include <sstream>

#include <sqlite3.h>

namespace squid {
namespace sqlite {

namespace {

std::string build_message(isqlite_api& api, const std::string& message, sqlite3& connection)
{
	const auto errmsg = api.errmsg(&connection);
	return message + ": " + (errmsg ? errmsg : "<null>");
}

std::string build_message(isqlite_api& api, const std::string& message, int errorCode)
{
	const auto errmsg = api.errstr(errorCode);
	return message + ": " + (errmsg ? errmsg : "<null>");
}

} // namespace

error::error(const std::string& message)
    : squid::error{ message }
    , ec_{}
{
}

error::error(isqlite_api& api, const std::string& message, sqlite3& connection)
    : squid::error{ build_message(api, message, connection) }
    , ec_{ api.errcode(&connection) }
{
}

error::error(isqlite_api& api, const std::string& message, int ec)
    : squid::error{ build_message(api, message, ec) }
    , ec_{ ec }
{
}

const std::optional<int>& error::ec() const
{
	return this->ec_;
}

} // namespace sqlite
} // namespace squid
