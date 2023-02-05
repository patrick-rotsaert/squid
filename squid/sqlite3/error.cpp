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

Error::Error(const std::string& message)
    : squid::Error{ message }
    , errorCode_()
{
}

Error::Error(const std::string& message, sqlite3& connection)
    : squid::Error{ build_message(message, connection) }
    , errorCode_(sqlite3_errcode(&connection))
{
}

Error::Error(const std::string& message, int errorCode)
    : squid::Error{ build_message(message, errorCode) }
    , errorCode_(errorCode)
{
}

const std::optional<int>& Error::errorCode() const
{
	return this->errorCode_;
}

} // namespace sqlite
} // namespace squid
