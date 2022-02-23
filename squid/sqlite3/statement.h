//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/ibackendstatement.h"

#include <memory>
#include <string>

#include <sqlite3.h>

namespace squid {
namespace sqlite {

class Statement final : public IBackendStatement
{
	std::shared_ptr<sqlite3>      connection_;
	std::string                   query_;
	std::shared_ptr<sqlite3_stmt> statement_;
	int                           stepResult_;

public:
	Statement(std::shared_ptr<sqlite3> connection, std::string_view query);

	Statement(const Statement&) = delete;
	Statement(Statement&& src)  = default;
	Statement& operator=(const Statement&) = delete;
	Statement& operator=(Statement&&) = default;

	void execute(const std::map<std::string, Parameter>& parameters) override;
	bool fetch(const std::vector<Result>& results) override;
};

} // namespace sqlite
} // namespace squid