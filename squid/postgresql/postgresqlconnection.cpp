//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "postgresqlconnection.h"
#include "postgresqlbackendconnectionfactory.h"

namespace squid {

PostgresqlConnection::PostgresqlConnection(std::string_view connectionInfo)
    : Connection{ PostgresqlBackendConnectionFactory{}, connectionInfo }
    , backend_{ std::dynamic_pointer_cast<PostgresqlBackendConnection>(this->backend()) }
{
}

const PostgresqlBackendConnection& PostgresqlConnection::backendConnection() const
{
	return *this->backend_;
}

} // namespace squid
