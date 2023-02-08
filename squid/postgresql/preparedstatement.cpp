//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/postgresql/preparedstatement.h"
#include "squid/postgresql/error.h"

#include "squid/postgresql/detail/query.h"
#include "squid/postgresql/detail/queryparameters.h"
#include "squid/postgresql/detail/connectionchecker.h"

#include <atomic>
#include <cstdint>
#include <cassert>

#include <libpq-fe.h>

namespace squid {
namespace postgresql {

std::string prepared_statement::next_statement_name()
{
	static std::atomic<std::uint64_t> statement_number = 0;
	return std::string{ "s_" } + std::to_string(++statement_number);
}

prepared_statement::prepared_statement(std::shared_ptr<PGconn> connection, std::string_view query)
    : basic_statement{ connection, query }
    , stmt_name_{ next_statement_name() }
    , prepared_{}
{
}

prepared_statement::~prepared_statement() noexcept
{
	try
	{
		if (this->prepared_)
		{
			std::shared_ptr<PGresult>{ PQexec(connection_checker::check(this->connection_), ("DEALLOCATE " + this->stmt_name_).c_str()),
				                       PQclear };
		}
	}
	catch (...)
	{
		;
	}
}

void prepared_statement::execute(const std::map<std::string, parameter>& parameters)
{
	this->exec_result_ = std::nullopt;

	if (!this->prepared_)
	{
		std::shared_ptr<PGresult> pgresult{ PQprepare(connection_checker::check(this->connection_),
			                                          this->stmt_name_.c_str(),
			                                          this->query_->query().c_str(),
			                                          this->query_->parameter_count(),
			                                          nullptr),
			                                PQclear };
		if (pgresult)
		{
			auto status = PQresultStatus(pgresult.get());
			if (PGRES_COMMAND_OK != status)
			{
				throw error{ "PQprepare failed", *this->connection_, *pgresult };
			}
			this->prepared_ = true;
		}
		else
		{
			throw error{ "PQprepare failed", *this->connection_ };
		}
	}

	query_parameters query_params{ *this->query_, parameters };

	assert(query_params.parameter_count() == this->query_->parameter_count());

	this->set_exec_result(std::shared_ptr<PGresult>{ PQexecPrepared(connection_checker::check(this->connection_),
	                                                                this->stmt_name_.c_str(),
	                                                                query_params.parameter_count(),
	                                                                query_params.parameter_values(),
	                                                                nullptr,
	                                                                nullptr,
	                                                                0),
	                                                 PQclear },
	                      "PQexecPrepared");
}

} // namespace postgresql
} // namespace squid
