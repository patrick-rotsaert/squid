//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/types.h"
#include "squid/detail/is_optional.h"

#include <variant>
#include <cstdint>
#include <string>
#include <optional>
#include <type_traits>

namespace squid {

/// This class holds a pointer to a bound result column.
class SQUID_EXPORT Result
{
public:
	using non_nullable_type = std::variant< //
	    bool*,
	    char*,
	    signed char*,
	    unsigned char*,
	    std::int16_t*,
	    std::uint16_t*,
	    std::int32_t*,
	    std::uint32_t*,
	    std::int64_t*,
	    std::uint64_t*,
	    float*,
	    double*,
	    long double*,
	    std::string*,
	    byte_string*,
	    time_point*,
	    date*,
	    time_of_day*
	    //
	    >;

	using nullable_type = std::variant< //
	    std::optional<bool>*,
	    std::optional<char>*,
	    std::optional<signed char>*,
	    std::optional<unsigned char>*,
	    std::optional<std::int16_t>*,
	    std::optional<std::uint16_t>*,
	    std::optional<std::int32_t>*,
	    std::optional<std::uint32_t>*,
	    std::optional<std::int64_t>*,
	    std::optional<std::uint64_t>*,
	    std::optional<float>*,
	    std::optional<double>*,
	    std::optional<long double>*,
	    std::optional<std::string>*,
	    std::optional<byte_string>*,
	    std::optional<time_point>*,
	    std::optional<date>*,
	    std::optional<time_of_day>*
	    //
	    >;

	using type = std::variant<non_nullable_type, nullable_type>;

	template<typename T>
	explicit Result(T& value)
	    : value_{ getValue(value) }
	{
	}

	Result(const Result&)            = default;
	Result(Result&& src)             = default;
	Result& operator=(const Result&) = delete;
	Result& operator=(Result&&)      = default;

	/// Get the value
	const type& value() const noexcept;

private:
	template<typename T>
	static type getValue(T& value)
	{
		if constexpr (is_optional_v<T>)
		{
			using V = typename T::value_type;
			if constexpr (std::is_enum_v<V>)
			{
				return nullable_type{ reinterpret_cast<std::optional<std::underlying_type_t<V>>*>(&value) };
			}
			else
			{
				return nullable_type{ &value };
			}
		}
		else if constexpr (std::is_enum_v<T>)
		{
			return non_nullable_type{ reinterpret_cast<std::underlying_type_t<T>*>(&value) };
		}
		else
		{
			return non_nullable_type{ &value };
		}
	}

private:
	type value_;
};

} // namespace squid
