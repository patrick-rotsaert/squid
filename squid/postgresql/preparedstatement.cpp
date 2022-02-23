//
// Copyright (C) 2022 Patrick Rotsaert
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

namespace squid {
namespace postgresql {

std::string PreparedStatement::nextStatementName()
{
	static std::atomic<std::uint64_t> statementNumber = 0;
	return std::string{ "s_" } + std::to_string(++statementNumber);
}

PreparedStatement::PreparedStatement(std::shared_ptr<PGconn> connection, std::string_view query)
    : BasicStatement{ connection, query }
    , stmtName_{ nextStatementName() }
    , prepared_{}
{
}

PreparedStatement::~PreparedStatement() noexcept
{
	try
	{
		if (this->prepared_)
		{
			std::shared_ptr<PGresult>{ PQexec(ConnectionChecker::check(this->connection_), ("DEALLOCATE " + this->stmtName_).c_str()),
				                       PQclear };
		}
	}
	catch (...)
	{
		;
	}
}

void PreparedStatement::execute(const std::map<std::string, Parameter>& parameters)
{
	this->execResult_ = std::nullopt;

	if (!this->prepared_)
	{
		std::shared_ptr<PGresult> pgResult{ PQprepare(ConnectionChecker::check(this->connection_),
			                                          this->stmtName_.c_str(),
			                                          this->query_->query().c_str(),
			                                          this->query_->nParams(),
			                                          nullptr),
			                                PQclear };
		if (pgResult)
		{
			auto status = PQresultStatus(pgResult.get());
			if (PGRES_COMMAND_OK != status)
			{
				throw Error{ "PQprepare failed", *this->connection_, *pgResult };
			}
			this->prepared_ = true;
		}
		else
		{
			throw Error{ "PQprepare failed", *this->connection_ };
		}
	}

	QueryParameters queryParameters{ *this->query_, parameters };

	assert(queryParameters.nParams() == this->query_->nParams());

	this->setExecResult(std::shared_ptr<PGresult>{ PQexecPrepared(ConnectionChecker::check(this->connection_),
	                                                              this->stmtName_.c_str(),
	                                                              queryParameters.nParams(),
	                                                              queryParameters.paramValues(),
	                                                              nullptr,
	                                                              nullptr,
	                                                              0),
	                                               PQclear },
	                    "PQexecPrepared");
}

} // namespace postgresql
} // namespace squid
