//
// Copyright (C) 2022-2023 Patrick Rotsaert
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

class no_connection_available : public error
{
public:
	no_connection_available();
};

class ibackend_connection;
class ibackend_connection_factory;
class connection_pool;

class SQUID_EXPORT connection
{
	std::shared_ptr<ibackend_connection> backend_;

public:
	/// Create a connection using the connection factory @a factory and a connection
	/// string @a connection_info passed to the backend.
	explicit connection(const ibackend_connection_factory& factory, std::string_view connection_info);

	/// Create a connection using the given @a backend
	explicit connection(std::shared_ptr<ibackend_connection>&& backend);

	/// Create a connection that acquires a backend connection from the @a pool.
	/// Waits indefinitely until the pool has a connection available.
	explicit connection(connection_pool& pool);

	/// Create a connection that acquires a backend connection from the @a pool with a given @a timeout.
	/// Throws @c no_connection_available if no connection is available within the specified timeout.
	explicit connection(connection_pool& pool, const std::chrono::milliseconds& timeout);

	/// Create a connection that acquires a backend connection from the @a pool.
	/// Returns std::nullopt immediately if no connection is available.
	static SQUID_EXPORT std::optional<connection> create(connection_pool& pool);

	/// Create a connection that acquires a backend connection from the @a pool with a given @a timeout.
	/// Returns std::nullopt if no connection is available within the specified timeout.
	static SQUID_EXPORT std::optional<connection> create(connection_pool& pool, const std::chrono::milliseconds& timeout);

	virtual ~connection() noexcept = default;

	connection(const connection&)            = delete;
	connection(connection&& src)             = default;
	connection& operator=(const connection&) = delete;
	connection& operator=(connection&&)      = default;

	/// Execute a statement without parameter nor result bindings.
	void execute(const std::string& query);

	/// Get the backend connection
	const std::shared_ptr<ibackend_connection>& backend() const;
};

} // namespace squid
