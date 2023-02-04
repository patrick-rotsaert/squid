//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/ibackendstatement.h"
#include "squid/postgresql/detail/libpqfwd.h"

#include <memory>
#include <string_view>

namespace squid {
namespace postgresql {

class Query;

/// Base class for Statement and PreparedStatement
/// Not intended to be instantiated directly.
class SQUID_EXPORT BasicStatement : public IBackendStatement
{
protected:
	struct ExecResult
	{
		std::shared_ptr<PGresult> pgResult;
		int                       rows;
		int                       currentRow;
	};

	std::shared_ptr<PGconn>   connection_;
	std::unique_ptr<Query>    query_;
	std::optional<ExecResult> execResult_;

	void setExecResult(std::shared_ptr<PGresult> pgResult, std::string_view execFunction);

public:
	BasicStatement(std::shared_ptr<PGconn> connection, std::string_view query);
	~BasicStatement() noexcept;

	BasicStatement(const BasicStatement&)            = delete;
	BasicStatement(BasicStatement&& src)             = default;
	BasicStatement& operator=(const BasicStatement&) = delete;
	BasicStatement& operator=(BasicStatement&&)      = default;

	bool fetch(const std::vector<Result>& results) override;
	bool fetch(const std::map<std::string, Result>& results) override;

	std::size_t getFieldCount() override;
	std::string getFieldName(std::size_t index) override;
};

} // namespace postgresql
} // namespace squid
