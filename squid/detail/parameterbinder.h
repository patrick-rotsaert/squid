//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <string_view>

namespace squid {

template<class StatementType>
class parameter_binder
{
	StatementType& st_;

public:
	explicit parameter_binder(StatementType& st)
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
class parameter_ref_binder
{
	StatementType& st_;

public:
	explicit parameter_ref_binder(StatementType& st)
	    : st_{ st }
	{
	}

	template<typename T>
	void bind(std::string_view name, const T& value)
	{
		this->st_.bind_ref(name, value);
	}
};

} // namespace squid
