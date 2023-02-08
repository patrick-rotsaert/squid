//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/postgresql/detail/query.h"

#include <cctype>
#include <cassert>

namespace squid {
namespace postgresql {

namespace {

inline bool is_name_char(char c)
{
	return std::isalpha(c) || c == '_';
}
} // namespace

postgresql_query::postgresql_query(std::string_view query)
    : query_{}
    , name_pos_map_{}
{
	// Implementation based on https://github.com/SOCI/soci/blob/master/src/backends/postgresql/statement.cpp,
	// simplified and improved.
	enum
	{
		normal,
		in_quotes,
		in_identifier,
		in_name
	} state = normal;

	auto name_begin       = query.end();
	int  parameter_number = 0;

	auto&& at_end_of_name = [this, &name_begin, &parameter_number](auto it) {
		assert(name_begin < it);
		std::string name{ name_begin, it };

		auto pos_it = this->name_pos_map_.find(name);
		if (pos_it == this->name_pos_map_.end())
		{
			this->name_pos_map_[name] = ++parameter_number;
			this->query_ += std::string{ "$" } + std::to_string(parameter_number);
		}
		else
		{
			this->query_ += std::string{ "$" } + std::to_string(pos_it->second);
		}
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
			else if (*it == ':')
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

const std::string& postgresql_query::query() const
{
	return this->query_;
}

int postgresql_query::parameter_count() const
{
	return static_cast<int>(this->name_pos_map_.size());
}

const std::map<std::string, int>& postgresql_query::parameter_name_pos_map() const
{
	return this->name_pos_map_;
}

} // namespace postgresql
} // namespace squid
