//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/result.h"

#include <vector>

#include <libpq-fe.h>

namespace squid {

class PostgresqlQueryResults final
{
public:
	static void store(const std::vector<Result>& results, const PGresult& pgResult, int row);
};

} // namespace squid
