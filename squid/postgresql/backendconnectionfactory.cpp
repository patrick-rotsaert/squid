//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/postgresql/backendconnectionfactory.h"
#include "squid/postgresql/backendconnection.h"

namespace squid {
namespace postgresql {

std::shared_ptr<IBackendConnection> BackendConnectionFactory::createBackendConnection(std::string_view connectionInfo) const
{
	return std::make_shared<BackendConnection>(std::string{ connectionInfo });
}

} // namespace postgresql
} // namespace squid
