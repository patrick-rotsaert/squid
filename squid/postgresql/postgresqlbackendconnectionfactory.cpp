//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "postgresqlbackendconnectionfactory.h"
#include "postgresqlbackendconnection.h"

namespace squid {

std::shared_ptr<IBackendConnection> PostgresqlBackendConnectionFactory::createBackendConnection(std::string_view connectionInfo) const
{
	return std::make_shared<PostgresqlBackendConnection>(std::string{ connectionInfo });
}

} // namespace squid
