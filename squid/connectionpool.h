//
// Copyright (C) 2022-2023 Patrick Rotsaert
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

class ibackend_connection;
class ibackend_connection_factory;

class SQUID_EXPORT connection_pool final
{
	class impl;
	std::unique_ptr<impl> pimpl_;

public:
	/// Create a pool of @a count connections using the connection factory @a factory and a connection
	/// string @a connection_info passed to the backend.
	connection_pool(const ibackend_connection_factory& factory, std::string_view connection_info, std::size_t count);
	~connection_pool() noexcept;

	connection_pool(const connection_pool&)            = delete;
	connection_pool(connection_pool&& src)             = default;
	connection_pool& operator=(const connection_pool&) = delete;
	connection_pool& operator=(connection_pool&&)      = default;

	/// Acquire a backend connection
	/// Waits indefinitely until the pool has a connection available.
	std::shared_ptr<ibackend_connection> acquire();

	/// Acquire a backend connection with timeout
	/// Returns nullptr if no connection is available within the specified timeout.
	std::shared_ptr<ibackend_connection> acquire(const std::chrono::milliseconds& timeout);

	/// Acquire a backend connection
	/// Immediately returns nullptr if no connection is available.
	std::shared_ptr<ibackend_connection> try_acquire();
};

} // namespace squid
