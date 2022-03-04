//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/parameter.h"
#include "squid/detail/always_false.h"

#include <cassert>

namespace squid {

Parameter::Parameter(const char* value, const ByValue&)
    : value_{ std::string_view{ value } }
{
}

const Parameter::pointer_type Parameter::pointer() const
{
	Parameter::pointer_type result;

	std::visit(
	    [&result](auto&& arg) {
		    using T = std::decay_t<decltype(arg)>;
		    if constexpr (std::is_same_v<T, value_type>)
		    {
			    std::visit(
			        [&result](auto&& arg) {
				        using T = std::decay_t<decltype(arg)>;
				        if constexpr (std::is_same_v<T, enum_char>)
				        {
					        result = enum_char_pointer{ &arg.value };
				        }
				        else
				        {
					        result = &arg;
				        }
			        },
			        arg);
		    }
		    else if constexpr (std::is_same_v<T, reference_type>)
		    {
			    std::visit(
			        [&result](auto&& arg) {
				        using T = std::decay_t<decltype(arg)>;
				        if constexpr (std::is_same_v<T, pointer_type>)
				        {
					        if (std::holds_alternative<enum_char_pointer>(arg))
					        {
						        validateEnumCharValue(*std::get<enum_char_pointer>(arg).value);
					        }
					        result = arg;
				        }
				        else if constexpr (std::is_same_v<T, pointer_optional_type>)
				        {
					        std::visit(
					            [&result](auto&& arg) {
						            using T = std::decay_t<decltype(arg)>;
						            if constexpr (std::is_same_v<T, enum_char_pointer_optional>)
						            {
							            if (arg.value->has_value())
							            {
								            validateEnumCharValue(arg.value->value());
								            result = enum_char_pointer{ &arg.value->value() };
							            }
							            else
							            {
								            result = &std::nullopt;
							            }
						            }
						            else
						            {
							            if (arg->has_value())
							            {
								            result = &arg->value();
							            }
							            else
							            {
								            result = &std::nullopt;
							            }
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

const Parameter::type& Parameter::value() const noexcept
{
	return this->value_;
}

char Parameter::validateEnumCharValue(char value)
{
	if (value < 0x20 || value > 0x7e)
	{
		throw Error{ "Binding a scoped enum with char as underlying type requires enum values within range [0x20 - 0x7E]" };
	}
	else
	{
		return value;
	}
}

} // namespace squid
