//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/parameter.h"

#include <string>
#include <vector>
#include <map>

namespace squid {
namespace postgresql {

class postgresql_query;

class query_parameters final
{
	std::vector<std::string> parameter_values_;
	std::vector<const char*> parameter_value_pointers_;

public:
	query_parameters(const postgresql_query& query, const std::map<std::string, parameter>& parameters);

	query_parameters(const query_parameters&)            = delete;
	query_parameters(query_parameters&& src)             = default;
	query_parameters& operator=(const query_parameters&) = delete;
	query_parameters& operator=(query_parameters&&)      = default;

	const char* const* parameter_values() const;

	int parameter_count() const;
};

} // namespace postgresql
} // namespace squid
