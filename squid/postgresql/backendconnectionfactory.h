//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/ibackendconnectionfactory.h"

namespace squid {
namespace postgresql {

class SQUID_EXPORT BackendConnectionFactory final : public IBackendConnectionFactory
{
	std::shared_ptr<IBackendConnection> createBackendConnection(std::string_view connectionInfo) const override;

public:
	BackendConnectionFactory()                                           = default;
	BackendConnectionFactory(const BackendConnectionFactory&)            = delete;
	BackendConnectionFactory(BackendConnectionFactory&& src)             = default;
	BackendConnectionFactory& operator=(const BackendConnectionFactory&) = delete;
	BackendConnectionFactory& operator=(BackendConnectionFactory&&)      = default;
};

} // namespace postgresql
} // namespace squid
