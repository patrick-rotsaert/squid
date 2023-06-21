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
	class impl;
	std::unique_ptr<impl> pimpl_;

public:
	~statement() noexcept;

	statement(std::shared_ptr<sqlite3> connection, std::string_view query, bool reuse_statement);

	statement(const statement&)            = delete;
	statement(statement&& src)             = default;
	statement& operator=(const statement&) = delete;
	statement& operator=(statement&&)      = default;

	void execute(const std::map<std::string, parameter>& parameters, const std::vector<result>& results) override;
	void execute(const std::map<std::string, parameter>& parameters, const std::map<std::string, result>& results) override;
	bool fetch() override;

	std::size_t field_count() override;
	std::string field_name(std::size_t index) override;

	std::uint64_t affected_rows() override;

	static void execute(sqlite3& connection, const std::string& query);
};

} // namespace sqlite
} // namespace squid
