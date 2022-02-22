//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/parameter.h"

#include <string>
#include <vector>
#include <map>

namespace squid {

class PostgresqlQuery;

class PostgresqlQueryParameters final
{
	std::vector<std::string> parameterValues;
	std::vector<const char*> parameterValuePointers;

public:
	PostgresqlQueryParameters(const PostgresqlQuery& query, const std::map<std::string, Parameter>& parameters);

	PostgresqlQueryParameters(const PostgresqlQueryParameters&) = delete;
	PostgresqlQueryParameters(PostgresqlQueryParameters&& src)  = default;
	PostgresqlQueryParameters& operator=(const PostgresqlQueryParameters&) = delete;
	PostgresqlQueryParameters& operator=(PostgresqlQueryParameters&&) = default;

	const char* const* paramValues() const;

	int nParams() const;
};

} // namespace squid
