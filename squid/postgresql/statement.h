//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/postgresql/basicstatement.h"

namespace squid {
namespace postgresql {

class statement final : public basic_statement
{
public:
	statement(std::shared_ptr<PGconn> connection, std::string_view query);

	statement(const statement&)            = delete;
	statement(statement&& src)             = default;
	statement& operator=(const statement&) = delete;
	statement& operator=(statement&&)      = default;

	void execute(const std::map<std::string, parameter>& parameters) override;
};

} // namespace postgresql
} // namespace squid
