//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "api.h"
#include "byte_string.h"

#include <variant>
#include <optional>
#include <cstdint>
#include <string_view>
#include <chrono>

namespace squid {

class SQUID_API Parameter
{
public:
	using value_type = std::variant<std::nullopt_t,
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
	                                std::string_view,
	                                std::string,
	                                byte_string_view,
	                                byte_string,
	                                std::chrono::system_clock::time_point,
	                                std::chrono::year_month_day,
	                                std::chrono::hh_mm_ss<std::chrono::microseconds>>;

	template<typename T>
	explicit Parameter(const T& value)
	    : value_{ value }
	{
	}

	explicit Parameter(const char* value);
	explicit Parameter(std::string value);
	explicit Parameter(byte_string value);
	explicit Parameter(const unsigned char* value, std::size_t size);

	Parameter(const Parameter&) = delete;
	Parameter(Parameter&& src)  = default;
	Parameter& operator=(const Parameter&) = delete;
	Parameter& operator=(Parameter&&) = default;

	const value_type& value() const noexcept;

private:
	value_type value_;
};

} // namespace squid
