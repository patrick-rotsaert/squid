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

class Query;

class QueryParameters final
{
	std::vector<std::string> parameterValues;
	std::vector<const char*> parameterValuePointers;

public:
	QueryParameters(const Query& query, const std::map<std::string, Parameter>& parameters);

	QueryParameters(const QueryParameters&)            = delete;
	QueryParameters(QueryParameters&& src)             = default;
	QueryParameters& operator=(const QueryParameters&) = delete;
	QueryParameters& operator=(QueryParameters&&)      = default;

	const char* const* paramValues() const;

	int nParams() const;
};

} // namespace postgresql
} // namespace squid
