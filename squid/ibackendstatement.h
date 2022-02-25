//
// Copyright (C) 2022 Patrick Rotsaert
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
class SQUID_EXPORT IBackendStatement
{
public:
	virtual ~IBackendStatement() noexcept = default;

	virtual void execute(const std::map<std::string, Parameter>& parameters) = 0;
	virtual bool fetch(const std::vector<Result>& results)                   = 0;
};

} // namespace squid
