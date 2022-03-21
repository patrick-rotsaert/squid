//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/ibackendconnection.h"
#include "squid/postgresql/detail/libpqfwd.h"

namespace squid {
namespace postgresql {

class SQUID_EXPORT BackendConnection final : public IBackendConnection
{
	std::shared_ptr<PGconn> connection_;

	std::unique_ptr<IBackendStatement> createStatement(std::string_view query) override;
	std::unique_ptr<IBackendStatement> createPreparedStatement(std::string_view query) override;
	void                               execute(const std::string& query) override;

public:
	/// @a connectionInfo must contain a valid PostgreSQL connection string
	explicit BackendConnection(const std::string& connectionInfo);

	BackendConnection(const BackendConnection&)            = delete;
	BackendConnection(BackendConnection&& src)             = default;
	BackendConnection& operator=(const BackendConnection&) = delete;
	BackendConnection& operator=(BackendConnection&&)      = default;

	PGconn& handle() const;
};

} // namespace postgresql
} // namespace squid
