//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "basicpostgresqlstatement.h"
#include "postgresqlerror.h"

#include "detail/postgresqlquery.h"
#include "detail/postgresqlqueryresults.h"

#include <cassert>

namespace squid {

void BasicPostgresqlStatement::setExecResult(std::shared_ptr<PGresult> pgResult, std::string_view execFunction)
{
	if (pgResult)
	{
		auto status = PQresultStatus(pgResult.get());
		if (PGRES_TUPLES_OK == status)
		{
			this->execResult_ = ExecResult{ .pgResult = pgResult, .rows = PQntuples(pgResult.get()), .currentRow = 0 };
		}
		else if (PGRES_COMMAND_OK == status)
		{
			this->execResult_ = ExecResult{ .pgResult = pgResult, .rows = 0, .currentRow = 0 };
		}
		else
		{
			throw PostgresqlError{ std::string{ execFunction } + " failed", *this->connection_, *pgResult.get() };
		}
	}
	else
	{
		throw PostgresqlError{ std::string{ execFunction } + " failed", *this->connection_ };
	}
}

BasicPostgresqlStatement::BasicPostgresqlStatement(std::shared_ptr<PGconn> connection, std::string_view query)
    : connection_{ std::move(connection) }
    , query_{ std::make_unique<PostgresqlQuery>(query) }
    , execResult_{}
{
	assert(this->connection_);
}

BasicPostgresqlStatement::~BasicPostgresqlStatement() noexcept
{
}

bool BasicPostgresqlStatement::fetch(const std::vector<Result>& results)
{
	if (!this->execResult_)
	{
		throw Error{ "Cannot fetch tuple from a statement that has not been executed" };
	}

	auto& execResult = this->execResult_.value();

	if (execResult.currentRow == execResult.rows)
	{
		return false;
	}

	PostgresqlQueryResults::store(results, *execResult.pgResult, execResult.currentRow++);

	return true;
}

} // namespace squid
