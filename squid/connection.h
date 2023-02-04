//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/error.h"

#include <memory>
#include <string_view>
#include <chrono>
#include <optional>

namespace squid {

class NoConnectionAvailable : public Error
{
public:
	NoConnectionAvailable();
};

class IBackendConnection;
class IBackendConnectionFactory;
class ConnectionPool;

class SQUID_EXPORT Connection
{
	std::shared_ptr<IBackendConnection> backend_;

public:
	/// Create a connection using the connection factory @a backendConnectionFactory and a connection
	/// string @a connectionInfo passed to the backend.
	explicit Connection(const IBackendConnectionFactory& backendConnectionFactory, std::string_view connectionInfo);

	/// Create a connection using the given @a backend
	explicit Connection(std::shared_ptr<IBackendConnection>&& backend);

	/// Create a connection that acquires a backend connection from the @a connectionPool.
	/// Waits indefinitely until the pool has a connection available.
	explicit Connection(ConnectionPool& connectionPool);

	/// Create a connection that acquires a backend connection from the @a connectionPool with a given @a timeout.
	/// Throws @c NoConnectionAvailable if no connection is available within the specified timeout.
	explicit Connection(ConnectionPool& connectionPool, const std::chrono::milliseconds& timeout);

	/// Create a connection that acquires a backend connection from the @a connectionPool.
	/// Returns std::nullopt immediately if no connection is available.
	static SQUID_EXPORT std::optional<Connection> create(ConnectionPool& connectionPool);

	/// Create a connection that acquires a backend connection from the @a connectionPool with a given @a timeout.
	/// Returns std::nullopt if no connection is available within the specified timeout.
	static SQUID_EXPORT std::optional<Connection> create(ConnectionPool& connectionPool, const std::chrono::milliseconds& timeout);

	virtual ~Connection() noexcept = default;

	Connection(const Connection&)            = delete;
	Connection(Connection&& src)             = default;
	Connection& operator=(const Connection&) = delete;
	Connection& operator=(Connection&&)      = default;

	/// Execute a statement without parameter nor result bindings.
	void execute(const std::string& query);

	/// Get the backend connection
	const std::shared_ptr<IBackendConnection>& backend() const;
};

} // namespace squid
