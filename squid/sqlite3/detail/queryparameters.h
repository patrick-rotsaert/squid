//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/parameter.h"
#include "squid/sqlite3/detail/sqlite3fwd.h"

#include <map>
#include <string>

namespace squid {
namespace sqlite {

class isqlite_api;

class query_parameters final
{
public:
	query_parameters() = delete;

	static void bind(isqlite_api& api, sqlite3& connection, sqlite3_stmt& statement, const std::map<std::string, parameter>& parameters);
};

} // namespace sqlite
} // namespace squid
