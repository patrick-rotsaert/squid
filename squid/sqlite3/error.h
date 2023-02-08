//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/error.h"
#include "squid/sqlite3/detail/sqlite3fwd.h"

#include <optional>
#include <string>

namespace squid {
namespace sqlite {

class SQUID_EXPORT error : public squid::error
{
	std::optional<int> ec_;

public:
	explicit error(const std::string& message);
	explicit error(const std::string& message, sqlite3& connection);
	explicit error(const std::string& message, int ec);

	const std::optional<int>& ec() const;
};

} // namespace sqlite
} // namespace squid
