//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/postgresql/basicstatement.h"
#include "squid/postgresql/error.h"

#include "squid/postgresql/detail/query.h"
#include "squid/postgresql/detail/queryresults.h"

#include <cassert>

#include <libpq-fe.h>

namespace squid {
namespace postgresql {

void BasicStatement::setExecResult(std::shared_ptr<PGresult> pgResult, std::string_view execFunction)
{
	if (pgResult)
	{
		const auto status = PQresultStatus(pgResult.get());
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
			throw Error{ std::string{ execFunction } + " failed", *this->connection_, *pgResult.get() };
		}
	}
	else
	{
		throw Error{ std::string{ execFunction } + " failed", *this->connection_ };
	}
}

BasicStatement::BasicStatement(std::shared_ptr<PGconn> connection, std::string_view query)
    : connection_{ std::move(connection) }
    , query_{ std::make_unique<Query>(query) }
    , execResult_{}
{
	assert(this->connection_);
}

BasicStatement::~BasicStatement() noexcept
{
}

bool BasicStatement::fetch(const std::vector<Result>& results)
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

	QueryResults::store(results, *execResult.pgResult, execResult.currentRow++);

	return true;
}

} // namespace postgresql
} // namespace squid
