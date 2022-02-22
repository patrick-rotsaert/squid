//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/error.h"

#include <optional>
#include <string>

#include <libpq-fe.h>

namespace squid {

class SQUID_API PostgresqlError : public Error
{
	std::optional<std::string> sqlState_;

public:
	explicit PostgresqlError(const std::string& message);
	explicit PostgresqlError(const std::string& message, const PGconn& connection);
	explicit PostgresqlError(const std::string& message, const PGconn& connection, const PGresult& result);

	const std::optional<std::string>& sqlState() const;
};

} // namespace squid
