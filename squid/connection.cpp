//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/connection.h"
#include "squid/connectionpool.h"
#include "squid/ibackendconnectionfactory.h"
#include "squid/ibackendconnection.h"

#include <cassert>

namespace squid {

NoConnectionAvailable::NoConnectionAvailable()
    : Error{ "No connection available within the given timeout" }
{
}

Connection::Connection(const IBackendConnectionFactory& backendConnectionFactory, std::string_view connectionInfo)
    : backend_{ backendConnectionFactory.createBackendConnection(connectionInfo) }
{
}

Connection::Connection(std::shared_ptr<IBackendConnection>&& backend)
    : backend_{ std::move(backend) }
{
}

Connection::Connection(ConnectionPool& connectionPool)
    : backend_{ connectionPool.acquire() }
{
	assert(this->backend_);
}

void Connection::execute(const std::string& query)
{
	this->backend_->execute(query);
}

Connection::Connection(ConnectionPool& connectionPool, const std::chrono::milliseconds& timeout)
    : backend_{ connectionPool.acquire(timeout) }
{
	if (!this->backend_)
	{
		throw NoConnectionAvailable{};
	}
}

std::optional<Connection> Connection::create(ConnectionPool& connectionPool)
{
	auto backend = connectionPool.tryAcquire();
	if (backend)
	{
		return Connection{ std::move(backend) };
	}
	else
	{
		return std::nullopt;
	}
}

std::optional<Connection> Connection::create(ConnectionPool& connectionPool, const std::chrono::milliseconds& timeout)
{
	auto backend = connectionPool.acquire(timeout);
	if (backend)
	{
		return Connection{ std::move(backend) };
	}
	else
	{
		return std::nullopt;
	}
}

const std::shared_ptr<IBackendConnection>& Connection::backend() const
{
	return this->backend_;
}

} // namespace squid
