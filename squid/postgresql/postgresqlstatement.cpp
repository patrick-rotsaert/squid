//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "postgresqlstatement.h"

#include "detail/postgresqlquery.h"
#include "detail/postgresqlqueryparameters.h"
#include "detail/postgresqlconnectionchecker.h"

namespace squid {

PostgresqlStatement::PostgresqlStatement(std::shared_ptr<PGconn> connection, std::string_view query)
    : BasicPostgresqlStatement{ connection, query }
{
}

void PostgresqlStatement::execute(const std::map<std::string, Parameter>& parameters)
{
	this->execResult_ = std::nullopt;

	PostgresqlQueryParameters queryParameters{ *this->query_, parameters };

	this->setExecResult(std::shared_ptr<PGresult>{ PQexecParams(PostgresqlConnectionChecker::check(this->connection_),
	                                                            this->query_->query().c_str(),
	                                                            queryParameters.nParams(),
	                                                            nullptr,
	                                                            queryParameters.paramValues(),
	                                                            nullptr,
	                                                            nullptr,
	                                                            0),
	                                               PQclear },
	                    "PQexecParams");
}

} // namespace squid
