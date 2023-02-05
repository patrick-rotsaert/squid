//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <string>
#include <string_view>
#include <map>

namespace squid {
namespace postgresql {

// PostgreSQL does not support named parameters, only ? and $n
// This class recreates the query string with all named parameters converted to $1, $2, $3, ...
class Query
{
	std::string                query_;
	std::map<std::string, int> namePosMap_;

public:
	explicit Query(std::string_view query);

	Query(const Query&)            = delete;
	Query(Query&& src)             = default;
	Query& operator=(const Query&) = delete;
	Query& operator=(Query&&)      = default;

	const std::string& query() const;

	int nParams() const;

	const std::map<std::string, int>& parameterNamePosMap() const;
};

} // namespace postgresql
} // namespace squid
