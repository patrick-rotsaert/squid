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

Connection::Connection(std::string_view connectionInfo)
    : squid::Connection{ BackendConnectionFactory{}, connectionInfo }
    , backend_{ std::dynamic_pointer_cast<BackendConnection>(this->backend()) }
{
}

const BackendConnection& Connection::backendConnection() const
{
	return *this->backend_;
}

} // namespace postgresql
} // namespace squid
