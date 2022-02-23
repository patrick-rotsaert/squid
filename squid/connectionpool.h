//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"

#include <memory>
#include <string_view>
#include <chrono>

namespace squid {

class IBackendConnection;
class IBackendConnectionFactory;

class SQUID_API ConnectionPool final
{
	class impl;
	std::unique_ptr<impl> pimpl_;

public:
	/// Create a pool of @a count connections using the connection factory @a backendConnectionFactory and a connection
	/// string @a connectionInfo passed to the backend.
	ConnectionPool(const IBackendConnectionFactory& backendConnectionFactory, std::string_view connectionInfo, std::size_t count);
	~ConnectionPool() noexcept = default;

	ConnectionPool(const ConnectionPool&) = delete;
	ConnectionPool(ConnectionPool&& src)  = default;
	ConnectionPool& operator=(const ConnectionPool&) = delete;
	ConnectionPool& operator=(ConnectionPool&&) = default;

	/// Waits indefinitely until the pool has a connection available.
	std::shared_ptr<IBackendConnection> acquire();

	/// Returns nullptr if no connection is available within the specified timeout.
	std::shared_ptr<IBackendConnection> acquire(const std::chrono::milliseconds& timeout);

	std::shared_ptr<IBackendConnection> tryAcquire();
};

} // namespace squid