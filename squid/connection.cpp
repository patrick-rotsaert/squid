//
// Copyright (C) 2022-2023 Patrick Rotsaert
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

no_connection_available::no_connection_available()
    : error{ "No connection available within the given timeout" }
{
}

connection::connection(const ibackend_connection_factory& factory, std::string_view connection_info)
    : backend_{ factory.create_backend_connection(connection_info) }
{
}

connection::connection(std::shared_ptr<ibackend_connection>&& backend)
    : backend_{ std::move(backend) }
{
}

connection::connection(connection_pool& pool)
    : backend_{ pool.acquire() }
{
	assert(this->backend_);
}

void connection::execute(const std::string& query)
{
	this->backend_->execute(query);
}

connection::connection(connection_pool& pool, const std::chrono::milliseconds& timeout)
    : backend_{ pool.acquire(timeout) }
{
	if (!this->backend_)
	{
		throw no_connection_available{};
	}
}

std::optional<connection> connection::create(connection_pool& pool)
{
	auto backend = pool.try_acquire();
	if (backend)
	{
		return connection{ std::move(backend) };
	}
	else
	{
		return std::nullopt;
	}
}

std::optional<connection> connection::create(connection_pool& pool, const std::chrono::milliseconds& timeout)
{
	auto backend = pool.acquire(timeout);
	if (backend)
	{
		return connection{ std::move(backend) };
	}
	else
	{
		return std::nullopt;
	}
}

const std::shared_ptr<ibackend_connection>& connection::backend() const
{
	return this->backend_;
}

} // namespace squid
