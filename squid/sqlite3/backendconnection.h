//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/ibackendconnection.h"

#include <sqlite3.h>

namespace squid {
namespace sqlite {

class SQUID_API BackendConnection final : public IBackendConnection
{
	std::shared_ptr<sqlite3> connection_;

	std::unique_ptr<IBackendStatement> createStatement(std::string_view query) override;
	std::unique_ptr<IBackendStatement> createPreparedStatement(std::string_view query) override;
	void                               execute(const std::string& query) override;

public:
	/// @a connectionInfo must contain a path to a file
	/// or ":memory:" for an in-memory database.
	/// Files that do not exist will be created.
	explicit BackendConnection(const std::string& connectionInfo);

	BackendConnection(const BackendConnection&) = delete;
	BackendConnection(BackendConnection&& src)  = default;
	BackendConnection& operator=(const BackendConnection&) = delete;
	BackendConnection& operator=(BackendConnection&&) = default;

	sqlite3& handle() const;
};

} // namespace sqlite
} // namespace squid
