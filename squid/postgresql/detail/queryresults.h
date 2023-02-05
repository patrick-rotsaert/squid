//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/result.h"
#include "squid/postgresql/detail/libpqfwd.h"

#include <vector>
#include <map>

namespace squid {
namespace postgresql {

class QueryResults final
{
public:
	static void store(const std::vector<Result>& results, const PGresult& pgResult, int row);
	static void store(const std::map<std::string, Result>& results, const PGresult& pgResult, int row);
};

} // namespace postgresql
} // namespace squid
