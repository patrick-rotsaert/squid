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

namespace squid {

class ibackend_connection;

/// Interface for a backend connection factory
class SQUID_EXPORT ibackend_connection_factory
{
public:
	virtual ~ibackend_connection_factory() noexcept = default;

	virtual std::shared_ptr<ibackend_connection> create_backend_connection(std::string_view connection_info) const = 0;
};

} // namespace squid
