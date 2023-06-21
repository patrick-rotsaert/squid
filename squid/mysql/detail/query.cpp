//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/mysql/detail/query.h"

#include <cctype>
#include <cassert>

namespace squid {
namespace mysql {

namespace {

inline bool is_name_char(char c)
{
	return std::isalpha(c) || c == '_';
}
} // namespace

mysql_query::mysql_query(std::string_view query)
    : query_{}
    , name_pos_map_{}
    , parameter_count_{}
{
	// Implementation based on https://github.com/SOCI/soci/blob/master/src/backends/mysql/statement.cpp,
	// simplified and improved.
	enum
	{
		normal,
		in_quotes,
		in_identifier,
		in_name
	} state = normal;

	auto name_begin = query.end();

	auto&& at_end_of_name = [this, &name_begin](auto it) {
		assert(name_begin < it);
		std::string name{ name_begin, it };

		this->name_pos_map_[name].push_back(this->parameter_count_++);
		this->query_.append("?");
	};

	for (auto it = query.begin(), end = query.end(); it != end; ++it)
	{
		switch (state)
		{
		case normal:
			if (*it == '\'')
			{
				this->query_ += *it;
				state = in_quotes;
			}
			else if (*it == '\"')
			{
				this->query_ += *it;
				state = in_identifier;
			}
			else if (*it == ':' || *it == '@' || *it == '$')
			{
				auto next = it + 1;
				if (next != end)
				{
					if (is_name_char(*next))
					{
						name_begin = next;
						state      = in_name;
					}
					else
					{
						this->query_ += *it++;
						this->query_ += *it;
					}
				}
				else
				{
					this->query_ += *it;
				}
			}
			else
			{
				this->query_ += *it;
			}
			break;
		case in_quotes:
			if (*it == '\'')
			{
				auto next = it + 1;
				if (next != end)
				{
					if (*next == '\'')
					{
						this->query_ += *it++;
						this->query_ += *it;
					}
					else
					{
						this->query_ += *it;
						state = normal;
					}
				}
				else
				{
					this->query_ += *it;
					state = normal;
				}
			}
			else
			{
				this->query_ += *it;
			}
			break;
		case in_identifier:
			if (*it == '"')
			{
				auto next = it + 1;
				if (next != end)
				{
					if (*next == '"')
					{
						this->query_ += *it++;
						this->query_ += *it;
					}
					else
					{
						this->query_ += *it;
						state = normal;
					}
				}
				else
				{
					this->query_ += *it;
					state = normal;
				}
			}
			else
			{
				this->query_ += *it;
			}
			break;
		case in_name:
			if (!is_name_char(*it))
			{
				at_end_of_name(it);
				state = normal;
				--it;
			}
			break;
		}
	}

	if (state == in_name)
	{
		at_end_of_name(query.end());
	}
}

const std::string& mysql_query::query() const
{
	return this->query_;
}

size_t mysql_query::parameter_count() const
{
	return this->parameter_count_;
}

const std::map<std::string, std::vector<size_t>>& mysql_query::parameter_name_pos_map() const
{
	return this->name_pos_map_;
}

} // namespace mysql
} // namespace squid
