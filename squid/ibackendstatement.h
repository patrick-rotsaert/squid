//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/parameter.h"
#include "squid/result.h"

#include <map>
#include <vector>
#include <string>

namespace squid {

/// Interface for a backend statement
class SQUID_EXPORT ibackend_statement
{
public:
	virtual ~ibackend_statement() noexcept = default;

	virtual void execute(const std::map<std::string, parameter>& parameters) = 0;
	virtual bool fetch(const std::vector<result>& results)                   = 0;
	virtual bool fetch(const std::map<std::string, result>& results)         = 0;

	virtual std::size_t field_count()                 = 0;
	virtual std::string field_name(std::size_t index) = 0;
};

} // namespace squid
