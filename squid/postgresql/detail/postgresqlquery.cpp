//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "postgresqlquery.h"

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

PostgresqlQuery::PostgresqlQuery(std::string_view query)
    : query_{}
    , namePosMap_{}
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

	auto nameBegin       = query.end();
	int  parameterNumber = 0;

	auto&& atEndOfName = [this, &nameBegin, &parameterNumber](auto it) {
		assert(nameBegin < it);
		std::string name{ nameBegin, it };

		auto posIt = this->namePosMap_.find(name);
		if (posIt == this->namePosMap_.end())
		{
			this->namePosMap_[name] = ++parameterNumber;
			this->query_ += std::string{ "$" } + std::to_string(parameterNumber);
		}
		else
		{
			this->query_ += std::string{ "$" } + std::to_string(posIt->second);
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
						nameBegin = next;
						state     = in_name;
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
				this->query_ += *it;
				state = normal;
			}
			// TODO: handle escaped quote
			else
			{
				this->query_ += *it;
			}
			break;
		case in_identifier:
			if (*it == '\"')
			{
				this->query_ += *it;
				state = normal;
			}
			// TODO: handle escaped quote
			else
			{
				this->query_ += *it;
			}
			break;
		case in_name:
			if (!is_name_char(*it))
			{
				atEndOfName(it);
				state = normal;
				--it;
			}
			break;
		}
	}

	if (state == in_name)
	{
		atEndOfName(query.end());
	}
}

const std::string& PostgresqlQuery::query() const
{
	return this->query_;
}

int PostgresqlQuery::nParams() const
{
	return static_cast<int>(this->namePosMap_.size());
}

const std::map<std::string, int>& PostgresqlQuery::parameterNamePosMap() const
{
	return this->namePosMap_;
}

} // namespace postgresql
} // namespace squid
