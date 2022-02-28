//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/byte_string.h"
#include "squid/error.h"
#include "squid/detail/is_optional.h"
#include "squid/detail/is_scoped_enum.h"

#include <variant>
#include <optional>
#include <cstdint>
#include <string_view>
#include <chrono>
#include <type_traits>

namespace squid {

/// This class holds a bound query parameter or a reference to it.
class SQUID_EXPORT Parameter
{
public:
	struct enum_char
	{
		char value;
	};

	using pointer_type = std::variant< //
	    const std::nullopt_t*,
	    const bool*,
	    const char*,
	    const signed char*,
	    const unsigned char*,
	    const std::int16_t*,
	    const std::uint16_t*,
	    const std::int32_t*,
	    const std::uint32_t*,
	    const std::int64_t*,
	    const std::uint64_t*,
	    const float*,
	    const double*,
	    const long double*,
	    const enum_char*,
	    const std::string_view*,
	    const std::string*,
	    const byte_string_view*,
	    const byte_string*,
	    const std::chrono::system_clock::time_point*,
	    const std::chrono::year_month_day*,
	    const std::chrono::hh_mm_ss<std::chrono::microseconds>*
	    //
	    >;

	// for tag dispatching
	struct ByValue
	{
	};

	// for tag dispatching
	struct ByReference
	{
	};

	template<typename T>
	explicit Parameter(const T& value, const ByValue&)
	    : value_{ getValue(value) }
	{
	}

	template<typename T>
	explicit Parameter(const T& value, const ByReference&)
	    : value_{ &value }
	{
	}

	/// Holds a std::string_view, string content is not copied.
	explicit Parameter(const char* value, const ByValue&);

	Parameter(const Parameter&) = delete;
	Parameter(Parameter&& src)  = default;
	Parameter& operator=(const Parameter&) = delete;
	Parameter& operator=(Parameter&&) = default;

	/// Get the value pointer
	const pointer_type pointer() const noexcept;

private:
	using value_type = std::variant< //
	    std::nullopt_t,
	    bool,
	    char,
	    signed char,
	    unsigned char,
	    std::int16_t,
	    std::uint16_t,
	    std::int32_t,
	    std::uint32_t,
	    std::int64_t,
	    std::uint64_t,
	    float,
	    double,
	    long double,
	    enum_char,
	    std::string_view,
	    std::string,
	    byte_string_view,
	    byte_string,
	    std::chrono::system_clock::time_point,
	    std::chrono::year_month_day,
	    std::chrono::hh_mm_ss<std::chrono::microseconds>
	    //
	    >;

	using pointer_optional_type = std::variant< //
	    const std::optional<bool>*,
	    const std::optional<char>*,
	    const std::optional<signed char>*,
	    const std::optional<unsigned char>*,
	    const std::optional<std::int16_t>*,
	    const std::optional<std::uint16_t>*,
	    const std::optional<std::int32_t>*,
	    const std::optional<std::uint32_t>*,
	    const std::optional<std::int64_t>*,
	    const std::optional<std::uint64_t>*,
	    const std::optional<float>*,
	    const std::optional<double>*,
	    const std::optional<long double>*,
	    const std::optional<enum_char>*,
	    const std::optional<std::string_view>*,
	    const std::optional<std::string>*,
	    const std::optional<byte_string_view>*,
	    const std::optional<byte_string>*,
	    const std::optional<std::chrono::system_clock::time_point>*,
	    const std::optional<std::chrono::year_month_day>*,
	    const std::optional<std::chrono::hh_mm_ss<std::chrono::microseconds>>*
	    //
	    >;

	using reference_type = std::variant<pointer_type, pointer_optional_type>;

	using type = std::variant<value_type, reference_type>;

private:
	template<typename T>
	static value_type getValue(const T& value)
	{
		if constexpr (is_optional_v<T>)
		{
			if (value.has_value())
			{
				return Parameter::getValue(value.value());
			}
			else
			{
				return value_type{ std::nullopt };
			}
		}
		else if constexpr (std::is_enum_v<T>)
		{
			using base = std::underlying_type_t<T>;
			if constexpr (std::is_scoped_enum_v<T> && std::is_same_v<base, char>)
			{
				const auto c = static_cast<char>(value);
				if (c < 0x20 || c > 0x7f)
				{
					throw Error{ "Binding a scoped enum with char as underlying type requires enum values within range [0x20 - 0x7F]" };
				}
				return value_type{ enum_char{ c } };
			}
			else
			{
				return value_type{ static_cast<base>(value) };
			}
		}
		else
		{
			return value_type{ value };
		}
	}

private:
	type value_;
};

} // namespace squid
