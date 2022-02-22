//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "basicpostgresqlstatement.h"

namespace squid {

class PostgresqlStatement final : public BasicPostgresqlStatement
{
public:
	PostgresqlStatement(std::shared_ptr<PGconn> connection, std::string_view query);

	PostgresqlStatement(const PostgresqlStatement&) = delete;
	PostgresqlStatement(PostgresqlStatement&& src)  = default;
	PostgresqlStatement& operator=(const PostgresqlStatement&) = delete;
	PostgresqlStatement& operator=(PostgresqlStatement&&) = default;

	void execute(const std::map<std::string, Parameter>& parameters) override;
};

} // namespace squid
