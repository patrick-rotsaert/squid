//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "basicpostgresqlstatement.h"

namespace squid {
namespace postgresql {

class Statement final : public BasicStatement
{
public:
	Statement(std::shared_ptr<PGconn> connection, std::string_view query);

	Statement(const Statement&) = delete;
	Statement(Statement&& src)  = default;
	Statement& operator=(const Statement&) = delete;
	Statement& operator=(Statement&&) = default;

	void execute(const std::map<std::string, Parameter>& parameters) override;
};

} // namespace postgresql
} // namespace squid
