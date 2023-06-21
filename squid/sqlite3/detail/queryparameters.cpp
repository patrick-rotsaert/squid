//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "queryparameters.h"

#include "squid/sqlite3/error.h"

#include "squid/detail/conversions.h"
#include "squid/detail/always_false.h"

#include <sqlite3.h>

#include <iomanip>
#include <cassert>

#ifdef SQUID_DEBUG_SQLITE
#include <iostream>
#endif

#include <sqlite3.h>

namespace squid {
namespace sqlite {

namespace {

void bind_parameter(sqlite3& connection, sqlite3_stmt& statement, const std::string& name, const parameter& parameter)
{
	auto tmp_name        = ":" + name;
	auto parameter_index = sqlite3_bind_parameter_index(&statement, tmp_name.c_str());
	if (parameter_index < 1)
	{
		// hacky!
		tmp_name.front() = '@';
		parameter_index  = sqlite3_bind_parameter_index(&statement, tmp_name.c_str());
		if (parameter_index < 1)
		{
			// hacky again ;)
			tmp_name.front() = '$';
			parameter_index  = sqlite3_bind_parameter_index(&statement, tmp_name.c_str());
			if (parameter_index < 1)
			{
				std::ostringstream msg;
				msg << "Parameter name " << std::quoted(name) << " was not found in the statement";
				throw error{ msg.str() };
			}
		}
	}
	assert(parameter_index > 0);

#define BIND0(f)                                                                                                                           \
	do                                                                                                                                     \
	{                                                                                                                                      \
		if (SQLITE_OK != f(&statement, parameter_index))                                                                                   \
		{                                                                                                                                  \
			throw error(#f " failed", connection);                                                                                         \
		}                                                                                                                                  \
	} while (false)

#define BIND(f, ...)                                                                                                                       \
	do                                                                                                                                     \
	{                                                                                                                                      \
		if (SQLITE_OK != f(&statement, parameter_index, __VA_ARGS__))                                                                      \
		{                                                                                                                                  \
			throw error(#f " failed", connection);                                                                                         \
		}                                                                                                                                  \
	} while (false)

	std::visit(
	    [&connection, &statement, &parameter_index](auto&& arg) {
		    using T = std::decay_t<decltype(arg)>;
		    if constexpr (std::is_same_v<T, const std::nullopt_t*>)
		    {
			    BIND0(sqlite3_bind_null);
		    }
		    else if constexpr (std::is_same_v<T, const bool*>)
		    {
			    BIND(sqlite3_bind_int, *arg ? 1 : 0);
		    }
		    else if constexpr (std::is_same_v<T, const char*>)
		    {
			    BIND(sqlite3_bind_text, arg, 1, SQLITE_STATIC);
		    }
		    else if constexpr (std::is_same_v<T, const signed char*> || std::is_same_v<T, const unsigned char*> ||
		                       std::is_same_v<T, const std::int16_t*> || std::is_same_v<T, const std::uint16_t*> ||
		                       std::is_same_v<T, const std::int32_t*>)
		    {
			    BIND(sqlite3_bind_int, static_cast<int>(*arg));
		    }
		    else if constexpr (std::is_same_v<T, const std::uint32_t*> || std::is_same_v<T, const std::int64_t*> ||
		                       std::is_same_v<T, const std::uint64_t*>)
		    {
			    BIND(sqlite3_bind_int64, static_cast<sqlite3_int64>(*arg));
		    }
		    else if constexpr (std::is_same_v<T, const float*> || std::is_same_v<T, const double*> || std::is_same_v<T, const long double*>)
		    {
			    BIND(sqlite3_bind_double, static_cast<double>(*arg));
		    }
		    else if constexpr (std::is_same_v<T, const std::string*> || std::is_same_v<T, const std::string_view*>)
		    {
			    BIND(sqlite3_bind_text, arg->data(), static_cast<int>(arg->length()), SQLITE_STATIC);
		    }
		    else if constexpr (std::is_same_v<T, const byte_string*> || std::is_same_v<T, const byte_string_view*>)
		    {
			    BIND(sqlite3_bind_blob, arg->data(), static_cast<int>(arg->length()), SQLITE_STATIC);
		    }
		    else if constexpr (std::is_same_v<T, const time_point*>)
		    {
			    auto tmp = time_point_to_string(*arg);
			    BIND(sqlite3_bind_text, tmp.data(), static_cast<int>(tmp.length()), SQLITE_TRANSIENT);
		    }
		    else if constexpr (std::is_same_v<T, const date*>)
		    {
			    auto tmp = date_to_string(*arg);
			    BIND(sqlite3_bind_text, tmp.data(), static_cast<int>(tmp.length()), SQLITE_TRANSIENT);
		    }
		    else if constexpr (std::is_same_v<T, const time_of_day*>)
		    {
			    auto tmp = time_of_day_to_string(*arg);
			    BIND(sqlite3_bind_text, tmp.data(), static_cast<int>(tmp.length()), SQLITE_TRANSIENT);
		    }
#ifdef SQUID_HAVE_BOOST_DATE_TIME
		    else if constexpr (std::is_same_v<T, const boost::posix_time::ptime*>)
		    {
			    auto tmp = boost_ptime_to_string(*arg);
			    BIND(sqlite3_bind_text, tmp.data(), static_cast<int>(tmp.length()), SQLITE_TRANSIENT);
		    }
		    else if constexpr (std::is_same_v<T, const boost::gregorian::date*>)
		    {
			    auto tmp = boost_date_to_string(*arg);
			    BIND(sqlite3_bind_text, tmp.data(), static_cast<int>(tmp.length()), SQLITE_TRANSIENT);
		    }
		    else if constexpr (std::is_same_v<T, const boost::posix_time::time_duration*>)
		    {
			    auto tmp = boost_time_duration_to_string(*arg);
			    BIND(sqlite3_bind_text, tmp.data(), static_cast<int>(tmp.length()), SQLITE_TRANSIENT);
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

/*static*/ void query_parameters::bind(sqlite3& connection, sqlite3_stmt& statement, const std::map<std::string, parameter>& parameters)
{
	for (const auto& pair : parameters)
	{
		bind_parameter(connection, statement, pair.first, pair.second);
	}
}

} // namespace sqlite
} // namespace squid
