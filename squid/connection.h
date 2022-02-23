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

namespace squid {

class IBackendConnection;
class IBackendConnectionFactory;

class SQUID_API Connection
{
	std::shared_ptr<IBackendConnection> backend_;

public:
	/// Create a connection using the connection factory @a backendConnectionFactory and a connection
	/// string @a connectionInfo passed to the backend.
	Connection(const IBackendConnectionFactory& backendConnectionFactory, std::string_view connectionInfo);

	virtual ~Connection() noexcept = default;

	Connection(const Connection&) = delete;
	Connection(Connection&& src)  = default;
	Connection& operator=(const Connection&) = delete;
	Connection& operator=(Connection&&) = default;

	/// Get the backend connection
	const std::shared_ptr<IBackendConnection>& backend() const;
};

} // namespace squid
