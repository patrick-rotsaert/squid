//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/ibackendconnection.h"

#include <libpq-fe.h>

namespace squid {

class SQUID_API PostgresqlBackendConnection final : public IBackendConnection
{
	std::shared_ptr<PGconn> connection_;

	std::unique_ptr<IBackendStatement> createStatement(std::string_view query) override;

	std::unique_ptr<IBackendStatement> createPreparedStatement(std::string_view query) override;

public:
	explicit PostgresqlBackendConnection(const std::string& connectionInfo);

	PostgresqlBackendConnection(const PostgresqlBackendConnection&) = delete;
	PostgresqlBackendConnection(PostgresqlBackendConnection&& src)  = default;
	PostgresqlBackendConnection& operator=(const PostgresqlBackendConnection&) = delete;
	PostgresqlBackendConnection& operator=(PostgresqlBackendConnection&&) = default;

	PGconn& handle() const;
};

} // namespace squid
