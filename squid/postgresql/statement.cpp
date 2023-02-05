//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/postgresql/statement.h"

#include "squid/postgresql/detail/query.h"
#include "squid/postgresql/detail/queryparameters.h"
#include "squid/postgresql/detail/connectionchecker.h"

#include <libpq-fe.h>

namespace squid {
namespace postgresql {

Statement::Statement(std::shared_ptr<PGconn> connection, std::string_view query)
    : BasicStatement{ connection, query }
{
}

void Statement::execute(const std::map<std::string, Parameter>& parameters)
{
	this->execResult_ = std::nullopt;

	QueryParameters queryParameters{ *this->query_, parameters };

	this->setExecResult(std::shared_ptr<PGresult>{ PQexecParams(ConnectionChecker::check(this->connection_),
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

} // namespace postgresql
} // namespace squid
