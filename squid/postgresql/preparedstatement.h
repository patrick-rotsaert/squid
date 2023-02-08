//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/postgresql/basicstatement.h"

namespace squid {
namespace postgresql {

class prepared_statement final : public basic_statement
{
	std::string stmt_name_;
	bool        prepared_;

	static std::string next_statement_name();

public:
	prepared_statement(std::shared_ptr<PGconn> connection, std::string_view query);

	~prepared_statement() noexcept;

	prepared_statement(const prepared_statement&)            = delete;
	prepared_statement(prepared_statement&& src)             = default;
	prepared_statement& operator=(const prepared_statement&) = delete;
	prepared_statement& operator=(prepared_statement&&)      = default;

	void execute(const std::map<std::string, parameter>& parameters) override;
};

} // namespace postgresql
} // namespace squid
