//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <string>
#include <string_view>
#include <map>

namespace squid {

// PostgreSQL does not support named parameters, only ? and $n
// This class recreates the query string with all named parameters converted to $1, $2, $3, ...
class PostgresqlQuery
{
	std::string                query_;
	std::map<std::string, int> namePosMap_;

public:
	explicit PostgresqlQuery(std::string_view query);

	PostgresqlQuery(const PostgresqlQuery&) = delete;
	PostgresqlQuery(PostgresqlQuery&& src)  = default;
	PostgresqlQuery& operator=(const PostgresqlQuery&) = delete;
	PostgresqlQuery& operator=(PostgresqlQuery&&) = default;

	const std::string& query() const;

	int nParams() const;

	const std::map<std::string, int>& parameterNamePosMap() const;
};

} // namespace squid
