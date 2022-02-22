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

class SQUID_API PostgresqlBackendConnectionFactory final : public IBackendConnectionFactory
{
	std::shared_ptr<IBackendConnection> createBackendConnection(std::string_view connectionInfo) const override;

public:
	PostgresqlBackendConnectionFactory()                                          = default;
	PostgresqlBackendConnectionFactory(const PostgresqlBackendConnectionFactory&) = delete;
	PostgresqlBackendConnectionFactory(PostgresqlBackendConnectionFactory&& src)  = default;
	PostgresqlBackendConnectionFactory& operator=(const PostgresqlBackendConnectionFactory&) = delete;
	PostgresqlBackendConnectionFactory& operator=(PostgresqlBackendConnectionFactory&&) = default;
};

} // namespace squid
