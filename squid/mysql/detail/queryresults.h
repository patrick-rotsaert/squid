//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/result.h"
#include "squid/mysql/detail/mysqlfwd.h"

#include <vector>
#include <map>
#include <memory>

namespace squid {
namespace mysql {

class query_results
{
	class column;

	std::shared_ptr<MYSQL_STMT>          statement_;
	std::shared_ptr<MYSQL_RES>           meta_;
	MYSQL_FIELD*                         fields_;
	size_t                               field_count_; // number of fields in the statement, may differ from columns_.size()
	std::vector<MYSQL_BIND>              binds_;
	std::vector<std::unique_ptr<column>> columns_;

	explicit query_results(std::shared_ptr<MYSQL_STMT> statement);

public:
	explicit query_results(std::shared_ptr<MYSQL_STMT> statement, const std::vector<result>& results);
	explicit query_results(std::shared_ptr<MYSQL_STMT> statement, const std::map<std::string, result>& results);

	~query_results() noexcept;

	size_t           field_count() const;
	std::string_view field_name(std::size_t index) const;

	bool fetch();
};

} // namespace mysql
} // namespace squid
