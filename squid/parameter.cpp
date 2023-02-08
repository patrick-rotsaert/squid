//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/parameter.h"
#include "squid/detail/always_false.h"

#include <cassert>

namespace squid {

parameter::parameter(const char* value, const by_value&)
    : value_{ std::string_view{ value } }
{
}

const parameter::pointer_type parameter::pointer() const
{
	parameter::pointer_type result;

	std::visit(
	    [&result](auto&& arg) {
		    using T = std::decay_t<decltype(arg)>;
		    if constexpr (std::is_same_v<T, value_type>)
		    {
			    std::visit([&result](auto&& arg) { result = &arg; }, arg);
		    }
		    else if constexpr (std::is_same_v<T, reference_type>)
		    {
			    std::visit(
			        [&result](auto&& arg) {
				        using T = std::decay_t<decltype(arg)>;
				        if constexpr (std::is_same_v<T, pointer_type>)
				        {
					        result = arg;
				        }
				        else if constexpr (std::is_same_v<T, pointer_optional_type>)
				        {
					        std::visit(
					            [&result](auto&& arg) {
						            if (arg->has_value())
						            {
							            result = &arg->value();
						            }
						            else
						            {
							            result = &std::nullopt;
						            }
					            },
					            arg);
				        }
				        else
				        {
					        static_assert(always_false_v<T>, "non-exhaustive visitor!");
				        }
			        },
			        arg);
		    }
		    else
		    {
			    static_assert(always_false_v<T>, "non-exhaustive visitor!");
		    }
	    },
	    this->value_);

	return result;
}

const parameter::type& parameter::value() const noexcept
{
	return this->value_;
}

} // namespace squid
