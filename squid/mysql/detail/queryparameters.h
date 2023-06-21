//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/parameter.h"

#include "squid/mysql/detail/mysqlfwd.h"

#include <string>
#include <vector>
#include <map>

namespace squid {
namespace mysql {

class mysql_query;

class query_parameters final
{
	std::vector<MYSQL_BIND>  binds_;
	std::vector<std::string> buffers_;

public:
	explicit query_parameters(const mysql_query& query, const std::map<std::string, parameter>& parameters);

	void bind(MYSQL_STMT& statement);
};

} // namespace mysql
} // namespace squid
