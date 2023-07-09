//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/sqlite3/connection.h"
#include "squid/sqlite3/backendconnection.h"
#include "squid/sqlite3/backendconnectionfactory.h"

#include "squid/sqlite3/detail/sqliteapi.h"

namespace squid {
namespace sqlite {

namespace {

sqlite_api g_api;

}

connection::connection(std::string_view connection_info)
    : connection{ g_api, connection_info }
{
}

connection::connection(isqlite_api& api, std::string_view connection_info)
    : squid::connection{ backend_connection_factory{ api }, connection_info }
    , backend_{ std::dynamic_pointer_cast<backend_connection>(this->squid::connection::backend()) }
{
}

const backend_connection& connection::backend() const
{
	return *this->backend_;
}

} // namespace sqlite
} // namespace squid
