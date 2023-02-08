//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/ibackendstatement.h"
#include "squid/sqlite3/detail/sqlite3fwd.h"

#include <memory>
#include <string>

namespace squid {
namespace sqlite {

class statement final : public ibackend_statement
{
	std::shared_ptr<sqlite3>      connection_;
	std::string                   query_;
	bool                          reuse_statement_;
	std::shared_ptr<sqlite3_stmt> statement_;
	int                           step_result_;

public:
	statement(std::shared_ptr<sqlite3> connection, std::string_view query, bool reuse_statement);

	statement(const statement&)            = delete;
	statement(statement&& src)             = default;
	statement& operator=(const statement&) = delete;
	statement& operator=(statement&&)      = default;

	void execute(const std::map<std::string, parameter>& parameters) override;
	bool fetch(const std::vector<result>& results) override;
	bool fetch(const std::map<std::string, result>& results) override;

	std::size_t field_count() override;
	std::string field_name(std::size_t index) override;

	static void execute(sqlite3& connection, const std::string& query);
};

} // namespace sqlite
} // namespace squid
