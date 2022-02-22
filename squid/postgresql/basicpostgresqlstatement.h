//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/ibackendstatement.h"

#include <memory>
#include <string_view>

#include <libpq-fe.h>

namespace squid {

class PostgresqlQuery;

class SQUID_API BasicPostgresqlStatement : public IBackendStatement
{
protected:
	struct ExecResult
	{
		std::shared_ptr<PGresult> pgResult;
		int                       rows;
		int                       currentRow;
	};

	std::shared_ptr<PGconn>          connection_;
	std::unique_ptr<PostgresqlQuery> query_;
	std::optional<ExecResult>        execResult_;

	void setExecResult(std::shared_ptr<PGresult> pgResult, std::string_view execFunction);

public:
	BasicPostgresqlStatement(std::shared_ptr<PGconn> connection, std::string_view query);
	~BasicPostgresqlStatement() noexcept;

	BasicPostgresqlStatement(const BasicPostgresqlStatement&) = delete;
	BasicPostgresqlStatement(BasicPostgresqlStatement&& src)  = default;
	BasicPostgresqlStatement& operator=(const BasicPostgresqlStatement&) = delete;
	BasicPostgresqlStatement& operator=(BasicPostgresqlStatement&&) = default;

	bool fetch(const std::vector<Result>& results) override;
};

} // namespace squid
