//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/postgresql/connection.h"
#include "squid/postgresql/backendconnection.h"
#include "squid/postgresql/backendconnectionfactory.h"

namespace squid {
namespace postgresql {

connection::connection(std::string_view connection_info)
    : squid::connection{ backend_connection_factory{}, connection_info }
    , backend_{ std::dynamic_pointer_cast<backend_connection>(this->squid::connection::backend()) }
{
}

const backend_connection& connection::backend() const
{
	return *this->backend_;
}

} // namespace postgresql
} // namespace squid
