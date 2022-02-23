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

class IBackendStatement;

/// Interface for a backend connection
class SQUID_API IBackendConnection
{
public:
	virtual ~IBackendConnection() noexcept = default;

	virtual std::unique_ptr<IBackendStatement> createStatement(std::string_view query)         = 0;
	virtual std::unique_ptr<IBackendStatement> createPreparedStatement(std::string_view query) = 0;
};

} // namespace squid
