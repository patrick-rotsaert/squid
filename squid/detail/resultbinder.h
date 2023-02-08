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
class result_binder
{
	StatementType& st_;

public:
	explicit result_binder(StatementType& st)
	    : st_{ st }
	{
	}

	template<typename T>
	void bind(std::string_view name, T& ref)
	{
		this->st_.bind_result(name, ref);
	}
};

} // namespace squid
