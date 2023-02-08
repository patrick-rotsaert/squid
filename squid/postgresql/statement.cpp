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

statement::statement(std::shared_ptr<PGconn> connection, std::string_view query)
    : basic_statement{ connection, query }
{
}

void statement::execute(const std::map<std::string, parameter>& parameters)
{
	this->exec_result_ = std::nullopt;

	query_parameters query_params{ *this->query_, parameters };

	this->set_exec_result(std::shared_ptr<PGresult>{ PQexecParams(connection_checker::check(this->connection_),
	                                                              this->query_->query().c_str(),
	                                                              query_params.parameter_count(),
	                                                              nullptr,
	                                                              query_params.parameter_values(),
	                                                              nullptr,
	                                                              nullptr,
	                                                              0),
	                                                 PQclear },
	                      "PQexecParams");
}

} // namespace postgresql
} // namespace squid
