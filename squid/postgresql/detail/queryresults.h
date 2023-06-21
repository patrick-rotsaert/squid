//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/result.h"
#include "squid/postgresql/detail/libpqfwd.h"

#include <vector>
#include <map>
#include <memory>

namespace squid {
namespace postgresql {

class query_results final
{
	struct column;

	std::shared_ptr<PGresult>            pgresult_;
	std::vector<std::unique_ptr<column>> columns_;
	size_t                               field_count_; // number of fields in the statement, may differ from columns_.size()

	explicit query_results(std::shared_ptr<PGresult> pgresult);

public:
	explicit query_results(std::shared_ptr<PGresult> pgresult, const std::vector<result>& results);
	explicit query_results(std::shared_ptr<PGresult> pgresult, const std::map<std::string, result>& results);

	~query_results() noexcept;

	size_t      field_count() const;
	std::string field_name(std::size_t index) const;

	void fetch(int row_index);
};

} // namespace postgresql
} // namespace squid
