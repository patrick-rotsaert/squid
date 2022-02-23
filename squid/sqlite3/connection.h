//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/connection.h"
#include "squid/api.h"

namespace squid {
namespace sqlite {

class BackendConnection;

// Convenience class to create a connection to an SQLite3 backend
// This class should be used if access to the native connection handle (sqlite3) is needed.
class SQUID_API Connection final : public squid::Connection
{
	std::shared_ptr<BackendConnection> backend_;

public:
	explicit Connection(std::string_view connectionInfo);

	Connection(const Connection&) = delete;
	Connection(Connection&& src)  = default;
	Connection& operator=(const Connection&) = delete;
	Connection& operator=(Connection&&) = default;

	/// Get the backend
	/// The backend provides a getter for the native connection handle (sqlite3)
	const BackendConnection& backendConnection() const;
};

} // namespace sqlite
} // namespace squid
