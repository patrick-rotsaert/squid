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
class ResultBinder
{
	StatementType& st_;

public:
	explicit ResultBinder(StatementType& st)
	    : st_{ st }
	{
	}

	template<typename T>
	void bind(std::string_view name, T& ref)
	{
		this->st_.bindResult(name, ref);
	}
};

} // namespace squid
