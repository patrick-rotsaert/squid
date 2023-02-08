//
// Copyright (C) 2022-2023 Patrick Rotsaert
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

void basic_statement::set_exec_result(std::shared_ptr<PGresult> pgresult, std::string_view exec_function)
{
	if (pgresult)
	{
		const auto status = PQresultStatus(pgresult.get());
		if (PGRES_TUPLES_OK == status)
		{
			this->exec_result_ = exec_result{ .pgresult = pgresult, .rows = PQntuples(pgresult.get()), .current_row = 0 };
		}
		else if (PGRES_COMMAND_OK == status)
		{
			this->exec_result_ = exec_result{ .pgresult = pgresult, .rows = 0, .current_row = 0 };
		}
		else
		{
			throw error{ std::string{ exec_function } + " failed", *this->connection_, *pgresult.get() };
		}
	}
	else
	{
		throw error{ std::string{ exec_function } + " failed", *this->connection_ };
	}
}

basic_statement::basic_statement(std::shared_ptr<PGconn> connection, std::string_view query)
    : connection_{ std::move(connection) }
    , query_{ std::make_unique<postgresql_query>(query) }
    , exec_result_{}
{
	assert(this->connection_);
}

basic_statement::~basic_statement() noexcept
{
}

bool basic_statement::fetch(const std::vector<result>& results)
{
	if (!this->exec_result_)
	{
		throw error{ "Cannot fetch tuple from a statement that has not been executed" };
	}

	auto& exec_result = this->exec_result_.value();

	if (exec_result.current_row == exec_result.rows)
	{
		return false;
	}

	query_results::store(results, *exec_result.pgresult, exec_result.current_row++);

	return true;
}

bool basic_statement::fetch(const std::map<std::string, result>& results)
{
	if (!this->exec_result_)
	{
		throw error{ "Cannot fetch tuple from a statement that has not been executed" };
	}

	auto& exec_result = this->exec_result_.value();

	if (exec_result.current_row == exec_result.rows)
	{
		return false;
	}

	query_results::store(results, *exec_result.pgresult, exec_result.current_row++);

	return true;
}

std::size_t basic_statement::field_count()
{
	if (!this->exec_result_)
	{
		throw error{ "Cannot get field count from a statement that has not been executed" };
	}

	const auto n = PQnfields(this->exec_result_.value().pgresult.get());
	if (n < 0)
	{
		throw error{ "PQnfields returned a negative value" };
	}

	return static_cast<std::size_t>(n);
}

std::string basic_statement::field_name(std::size_t index)
{
	if (!this->exec_result_)
	{
		throw error{ "Cannot get field count from a statement that has not been executed" };
	}

	const auto name = PQfname(this->exec_result_.value().pgresult.get(), index);
	if (name == nullptr)
	{
		throw error{ "PQfname returned a null pointer" };
	}

	return name;
}

} // namespace postgresql
} // namespace squid
