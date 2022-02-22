//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "api.h"

#include <stdexcept>
#include <string>

namespace squid {

class SQUID_API Error : public std::runtime_error
{
public:
	explicit Error(const std::string& message);
};

} // namespace squid
