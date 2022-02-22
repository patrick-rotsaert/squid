//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "basicpostgresqlstatement.h"

namespace squid {

class PostgresqlPreparedStatement final : public BasicPostgresqlStatement
{
	std::string stmtName_;
	bool        prepared_;

	static std::string nextStatementName();

public:
	PostgresqlPreparedStatement(std::shared_ptr<PGconn> connection, std::string_view query);

	~PostgresqlPreparedStatement() noexcept;

	PostgresqlPreparedStatement(const PostgresqlPreparedStatement&) = delete;
	PostgresqlPreparedStatement(PostgresqlPreparedStatement&& src)  = default;
	PostgresqlPreparedStatement& operator=(const PostgresqlPreparedStatement&) = delete;
	PostgresqlPreparedStatement& operator=(PostgresqlPreparedStatement&&) = default;

	void execute(const std::map<std::string, Parameter>& parameters) override;
};

} // namespace squid
