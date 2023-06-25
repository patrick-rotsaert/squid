//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "queryparameters.h"
#include "query.h"
#include "conversions.h"

#include "squid/mysql/error.h"

#include "squid/detail/always_false.h"

#include <cassert>

#include <mysql/mysql.h>

namespace squid {
namespace mysql {

namespace {

void bind_parameter(MYSQL_BIND& bind, std::string& buffer, const parameter& parameter)
{
	std::visit(
	    [&bind, &buffer](auto&& arg) {
		    using T = std::decay_t<decltype(arg)>;
		    if constexpr (std::is_same_v<T, const std::nullopt_t*>)
		    {
			    bind.buffer_type = MYSQL_TYPE_NULL;
		    }
		    else if constexpr (std::is_same_v<T, const bool*>)
		    {
			    bind.buffer_type = MYSQL_TYPE_TINY;
			    bind.buffer      = const_cast<void*>(reinterpret_cast<const void*>(arg));
		    }
		    else if constexpr (std::is_same_v<T, const char*>)
		    {
			    bind.buffer_type   = MYSQL_TYPE_STRING;
			    bind.buffer        = const_cast<void*>(reinterpret_cast<const void*>(arg));
			    bind.buffer_length = 1;
		    }
		    else if constexpr (std::is_same_v<T, const signed char*> || std::is_same_v<T, const unsigned char*>)
		    {
			    static_assert(sizeof(*arg) == 1u, "Size of [signed/unsigned] char must be 1");
			    bind.buffer_type = MYSQL_TYPE_TINY;
			    bind.is_unsigned = std::is_unsigned_v<std::remove_cvref_t<decltype(*arg)>>;
			    bind.buffer      = const_cast<void*>(reinterpret_cast<const void*>(arg));
		    }
		    else if constexpr (std::is_same_v<T, const std::int16_t*> || std::is_same_v<T, const std::uint16_t*>)
		    {
			    bind.buffer_type = MYSQL_TYPE_SHORT;
			    bind.is_unsigned = std::is_unsigned_v<std::remove_cvref_t<decltype(*arg)>>;
			    bind.buffer      = const_cast<void*>(reinterpret_cast<const void*>(arg));
		    }
		    else if constexpr (std::is_same_v<T, const std::int32_t*> || std::is_same_v<T, const std::uint32_t*>)
		    {
			    bind.buffer_type = MYSQL_TYPE_LONG;
			    bind.is_unsigned = std::is_unsigned_v<std::remove_cvref_t<decltype(*arg)>>;
			    bind.buffer      = const_cast<void*>(reinterpret_cast<const void*>(arg));
		    }
		    else if constexpr (std::is_same_v<T, const std::int64_t*> || std::is_same_v<T, const std::uint64_t*>)
		    {
			    bind.buffer_type = MYSQL_TYPE_LONGLONG;
			    bind.is_unsigned = std::is_unsigned_v<std::remove_cvref_t<decltype(*arg)>>;
			    bind.buffer      = const_cast<void*>(reinterpret_cast<const void*>(arg));
		    }
		    else if constexpr (std::is_same_v<T, const float*>)
		    {
			    static_assert(sizeof(*arg) == 4u, "Size of float must be 4");
			    bind.buffer_type = MYSQL_TYPE_FLOAT;
			    bind.buffer      = const_cast<void*>(reinterpret_cast<const void*>(arg));
		    }
		    else if constexpr (std::is_same_v<T, const double*>)
		    {
			    static_assert(sizeof(*arg) == 8u, "Size of double must be 8");
			    bind.buffer_type = MYSQL_TYPE_DOUBLE;
			    bind.buffer      = const_cast<void*>(reinterpret_cast<const void*>(arg));
		    }
		    else if constexpr (std::is_same_v<T, const long double*>)
		    {
			    static_assert(sizeof(double) == 8u, "Size of double must be 8");
			    buffer.resize(8u);
			    bind.buffer = reinterpret_cast<void*>(buffer.data());
			    auto& data  = *reinterpret_cast<double*>(buffer.data());
			    data        = static_cast<double>(*arg);
		    }
		    else if constexpr (std::is_same_v<T, const std::string*> || std::is_same_v<T, const std::string_view*>)
		    {
			    bind.buffer_type   = MYSQL_TYPE_STRING;
			    bind.buffer        = const_cast<void*>(reinterpret_cast<const void*>(arg->data()));
			    bind.buffer_length = static_cast<unsigned long>(arg->length());
		    }
		    else if constexpr (std::is_same_v<T, const byte_string*> || std::is_same_v<T, const byte_string_view*>)
		    {
			    bind.buffer_type   = MYSQL_TYPE_BLOB;
			    bind.buffer        = const_cast<void*>(reinterpret_cast<const void*>(arg->data()));
			    bind.buffer_length = static_cast<unsigned long>(arg->length());
		    }
		    else if constexpr (std::is_same_v<T, const time_point*>)
		    {
			    bind.buffer_type = MYSQL_TYPE_DATETIME;
			    buffer.resize(sizeof(MYSQL_TIME));
			    bind.buffer        = reinterpret_cast<void*>(buffer.data());
			    bind.buffer_length = static_cast<unsigned long>(buffer.size());
			    to_mysql_time(*arg, *reinterpret_cast<MYSQL_TIME*>(buffer.data()));
		    }
		    else if constexpr (std::is_same_v<T, const date*>)
		    {
			    bind.buffer_type = MYSQL_TYPE_DATE;
			    buffer.resize(sizeof(MYSQL_TIME));
			    bind.buffer        = reinterpret_cast<void*>(buffer.data());
			    bind.buffer_length = static_cast<unsigned long>(buffer.size());
			    to_mysql_time(*arg, *reinterpret_cast<MYSQL_TIME*>(buffer.data()));
		    }
		    else if constexpr (std::is_same_v<T, const time_of_day*>)
		    {
			    bind.buffer_type = MYSQL_TYPE_TIME;
			    buffer.resize(sizeof(MYSQL_TIME));
			    bind.buffer        = reinterpret_cast<void*>(buffer.data());
			    bind.buffer_length = static_cast<unsigned long>(buffer.size());
			    to_mysql_time(*arg, *reinterpret_cast<MYSQL_TIME*>(buffer.data()));
		    }
#ifdef SQUID_HAVE_BOOST_DATE_TIME
		    else if constexpr (std::is_same_v<T, const boost::posix_time::ptime*>)
		    {
			    bind.buffer_type = MYSQL_TYPE_DATETIME;
			    buffer.resize(sizeof(MYSQL_TIME));
			    bind.buffer        = reinterpret_cast<void*>(buffer.data());
			    bind.buffer_length = static_cast<unsigned long>(buffer.size());
			    to_mysql_time(*arg, *reinterpret_cast<MYSQL_TIME*>(buffer.data()));
		    }
		    else if constexpr (std::is_same_v<T, const boost::gregorian::date*>)
		    {
			    bind.buffer_type = MYSQL_TYPE_DATE;
			    buffer.resize(sizeof(MYSQL_TIME));
			    bind.buffer        = reinterpret_cast<void*>(buffer.data());
			    bind.buffer_length = static_cast<unsigned long>(buffer.size());
			    to_mysql_time(*arg, *reinterpret_cast<MYSQL_TIME*>(buffer.data()));
		    }
		    else if constexpr (std::is_same_v<T, const boost::posix_time::time_duration*>)
		    {
			    bind.buffer_type = MYSQL_TYPE_TIME;
			    buffer.resize(sizeof(MYSQL_TIME));
			    bind.buffer        = reinterpret_cast<void*>(buffer.data());
			    bind.buffer_length = static_cast<unsigned long>(buffer.size());
			    to_mysql_time(*arg, *reinterpret_cast<MYSQL_TIME*>(buffer.data()));
		    }
#endif
		    else
		    {
			    static_assert(always_false_v<T>, "non-exhaustive visitor!");
		    }
	    },
	    parameter.pointer());
}

} // namespace

query_parameters::query_parameters(const mysql_query& query, const std::map<std::string, parameter>& parameters)
    : binds_{ query.parameter_count() }
    , buffers_{ binds_.size() }
{
	for (const auto& pair : query.parameter_name_pos_map())
	{
		auto it = parameters.find(pair.first);
		if (it == parameters.end())
		{
			throw error{ "The query parameter '" + pair.first + "' is not bound" };
		}
		const auto& parameter = it->second;

		const auto& positions = pair.second;
		assert(!positions.empty());

		auto first = true;
		for (const auto position : positions)
		{
			assert(position < this->binds_.size());
			if (first)
			{
				bind_parameter(this->binds_[position], this->buffers_[position], parameter);
				first = false;
			}
			else
			{
				this->binds_[position] = this->binds_[positions.front()];
			}
		}
	}
}

void query_parameters::bind(MYSQL_STMT& statement)
{
	const auto param_count = mysql_stmt_param_count(&statement);
	if (static_cast<size_t>(param_count) != this->binds_.size())
	{
		throw error{ "The number of parameters returned by mysql_stmt_param_count (" + std::to_string(param_count) +
			         ") differs from the number of parsed parameters (" + std::to_string(this->binds_.size()) + ")" };
	}

	if (mysql_stmt_bind_param(&statement, &this->binds_.front()))
	{
		throw error{ "mysql_stmt_bind_param failed", statement };
	}
}

const std::vector<MYSQL_BIND>& query_parameters::binds() const
{
	return this->binds_;
}

} // namespace mysql
} // namespace squid
