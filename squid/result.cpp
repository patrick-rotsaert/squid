//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/result.h"

namespace squid {

const Result::value_type& Result::value() const noexcept
{
	return this->value_;
}

} // namespace squid
