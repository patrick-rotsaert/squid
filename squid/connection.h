//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "api.h"

#include <memory>
#include <string_view>

namespace squid {

class IBackendConnection;
class IBackendConnectionFactory;

class SQUID_API Connection
{
	std::shared_ptr<IBackendConnection> backend_;

public:
	Connection(const IBackendConnectionFactory& backendConnectionFactory, std::string_view connectionInfo);

	virtual ~Connection() noexcept = default;

	Connection(const Connection&) = delete;
	Connection(Connection&& src)  = default;
	Connection& operator=(const Connection&) = delete;
	Connection& operator=(Connection&&) = default;

	const std::shared_ptr<IBackendConnection>& backend() const;
};

} // namespace squid
