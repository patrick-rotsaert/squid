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
#include <vector>

namespace squid {
namespace mysql {

// mysql does not support named parameters, only ?
// This class recreates the query string with all named parameters converted to ?, ?, ...
class mysql_query
{
	std::string                                query_;
	std::map<std::string, std::vector<size_t>> name_pos_map_;
	size_t                                     parameter_count_;

public:
	explicit mysql_query(std::string_view query);

	mysql_query(const mysql_query&)            = delete;
	mysql_query(mysql_query&& src)             = default;
	mysql_query& operator=(const mysql_query&) = delete;
	mysql_query& operator=(mysql_query&&)      = default;

	const std::string& query() const;

	size_t parameter_count() const;

	const std::map<std::string, std::vector<size_t>>& parameter_name_pos_map() const;
};

} // namespace mysql
} // namespace squid
