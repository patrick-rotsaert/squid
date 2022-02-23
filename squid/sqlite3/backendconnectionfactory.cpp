//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/sqlite3/backendconnectionfactory.h"
#include "squid/sqlite3/backendconnection.h"

namespace squid {
namespace sqlite {

std::shared_ptr<IBackendConnection> BackendConnectionFactory::createBackendConnection(std::string_view connectionInfo) const
{
	return std::make_shared<BackendConnection>(std::string{ connectionInfo });
}

} // namespace sqlite
} // namespace squid
