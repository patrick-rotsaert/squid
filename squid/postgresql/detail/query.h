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
class postgresql_query
{
	std::string                query_;
	std::map<std::string, int> name_pos_map_;

public:
	explicit postgresql_query(std::string_view query);

	postgresql_query(const postgresql_query&)            = delete;
	postgresql_query(postgresql_query&& src)             = default;
	postgresql_query& operator=(const postgresql_query&) = delete;
	postgresql_query& operator=(postgresql_query&&)      = default;

	const std::string& query() const;

	int parameter_count() const;

	const std::map<std::string, int>& parameter_name_pos_map() const;
};

} // namespace postgresql
} // namespace squid
