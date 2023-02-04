//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <string_view>

namespace squid {

template<class StatementType>
class ParameterBinder
{
	StatementType& st_;

public:
	explicit ParameterBinder(StatementType& st)
	    : st_{ st }
	{
	}

	template<typename T>
	void bind(std::string_view name, const T& value)
	{
		this->st_.bind(name, value);
	}
};

template<class StatementType>
class ParameterRefBinder
{
	StatementType& st_;

public:
	explicit ParameterRefBinder(StatementType& st)
	    : st_{ st }
	{
	}

	template<typename T>
	void bind(std::string_view name, const T& value)
	{
		this->st_.bindRef(name, value);
	}
};

} // namespace squid
