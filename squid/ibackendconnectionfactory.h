//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "api.h"

#include <memory>
#include <string_view>

namespace squid {

class IBackendConnection;

/// Interface for a backend connection factory
class SQUID_API IBackendConnectionFactory
{
public:
	virtual ~IBackendConnectionFactory() noexcept = default;

	virtual std::shared_ptr<IBackendConnection> createBackendConnection(std::string_view connectionInfo) const = 0;
};

} // namespace squid
