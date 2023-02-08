//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/error.h"
#include "squid/postgresql/detail/libpqfwd.h"

#include <optional>
#include <string>

namespace squid {
namespace postgresql {

class SQUID_EXPORT error : public squid::error
{
	std::optional<std::string> sql_state_;

public:
	explicit error(const std::string& message);
	explicit error(const std::string& message, const PGconn& connection);
	explicit error(const std::string& message, const PGconn& connection, const PGresult& result);

	const std::optional<std::string>& sql_state() const;
};

} // namespace postgresql
} // namespace squid
