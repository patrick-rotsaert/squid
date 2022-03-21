//
// Copyright (C) 2022 Patrick Rotsaert
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

class SQUID_EXPORT Error : public squid::Error
{
	std::optional<int> errorCode_;

public:
	explicit Error(const std::string& message);
	explicit Error(const std::string& message, sqlite3& connection);
	explicit Error(const std::string& message, int errorCode);

	const std::optional<int>& errorCode() const;
};

} // namespace sqlite
} // namespace squid
