//
// Copyright (C) 2022-2023 Patrick Rotsaert
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

class postgresql_query;

/// Base class for statement and prepared_statement
/// Not intended to be instantiated directly.
class SQUID_EXPORT basic_statement : public ibackend_statement
{
protected:
	struct exec_result
	{
		std::shared_ptr<PGresult> pgresult;
		int                       rows;
		int                       current_row;
	};

	std::shared_ptr<PGconn>           connection_;
	std::unique_ptr<postgresql_query> query_;
	std::optional<exec_result>        exec_result_;

	void set_exec_result(std::shared_ptr<PGresult> pgresult, std::string_view exec_function);

public:
	basic_statement(std::shared_ptr<PGconn> connection, std::string_view query);
	~basic_statement() noexcept;

	basic_statement(const basic_statement&)            = delete;
	basic_statement(basic_statement&& src)             = default;
	basic_statement& operator=(const basic_statement&) = delete;
	basic_statement& operator=(basic_statement&&)      = default;

	bool fetch(const std::vector<result>& results) override;
	bool fetch(const std::map<std::string, result>& results) override;

	std::size_t field_count() override;
	std::string field_name(std::size_t index) override;
};

} // namespace postgresql
} // namespace squid
