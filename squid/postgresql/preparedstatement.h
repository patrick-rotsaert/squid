//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/postgresql/basicstatement.h"

namespace squid {
namespace postgresql {

class PreparedStatement final : public BasicStatement
{
	std::string stmtName_;
	bool        prepared_;

	static std::string nextStatementName();

public:
	PreparedStatement(std::shared_ptr<PGconn> connection, std::string_view query);

	~PreparedStatement() noexcept;

	PreparedStatement(const PreparedStatement&)            = delete;
	PreparedStatement(PreparedStatement&& src)             = default;
	PreparedStatement& operator=(const PreparedStatement&) = delete;
	PreparedStatement& operator=(PreparedStatement&&)      = default;

	void execute(const std::map<std::string, Parameter>& parameters) override;
};

} // namespace postgresql
} // namespace squid
