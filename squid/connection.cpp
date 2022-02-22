//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.h"
#include "ibackendconnectionfactory.h"

namespace squid {

Connection::Connection(const IBackendConnectionFactory& backendConnectionFactory, std::string_view connectionInfo)
    : backend_{ backendConnectionFactory.createBackendConnection(connectionInfo) }
{
}

const std::shared_ptr<IBackendConnection>& Connection::backend() const
{
	return this->backend_;
}

} // namespace squid
