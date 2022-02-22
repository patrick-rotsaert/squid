//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "parameter.h"
#include "detail/always_false.h"

namespace squid {

Parameter::Parameter(const char* value)
    : value_{ std::string_view{ value } }
{
}

Parameter::Parameter(std::string value)
    : value_{ std::move(value) }
{
}

Parameter::Parameter(byte_string value)
    : value_{ std::move(value) }
{
}

Parameter::Parameter(const unsigned char* value, std::size_t size)
    : value_{ byte_string_view{ value, size } }
{
}

const Parameter::value_type& Parameter::value() const noexcept
{
	return this->value_;
}

} // namespace squid
