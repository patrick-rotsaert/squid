//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/postgresql/detail/queryresults.h"
#include "squid/postgresql/detail/conversions.h"

#include "squid/postgresql/error.h"

#include "squid/detail/conversions.h"
#include "squid/detail/always_false.h"
#include "squid/detail/demangled_type_name.h"

#include <cassert>
#include <stdexcept>
#include <sstream>
#include <iomanip>

#include <libpq-fe.h>

namespace squid {
namespace postgresql {

namespace {

void store_result(const result::non_nullable_type& result, std::string_view column_name, std::string_view value)
{
	std::visit(
	    [&](auto&& arg) {
		    auto& destination = *arg;
		    using T           = std::decay_t<decltype(destination)>;
		    try
		    {
			    if constexpr (std::is_same_v<T, bool>)
			    {
				    if (value == "t")
				    {
					    destination = true;
				    }
				    else if (value == "f")
				    {
					    destination = false;
				    }
				    else
				    {
					    throw std::runtime_error{ "value not 't' nor 'f'" };
				    }
			    }
			    else if constexpr (std::is_same_v<T, char>)
			    {
				    if (value.length() != 1)
				    {
					    throw std::runtime_error{ "length is not 1" };
				    }
				    else
				    {
					    destination = value.front();
				    }
			    }
			    else if constexpr (std::is_same_v<T, signed char> || std::is_same_v<T, unsigned char> || std::is_same_v<T, std::int16_t> ||
			                       std::is_same_v<T, std::uint16_t> || std::is_same_v<T, std::int32_t> ||
			                       std::is_same_v<T, std::uint32_t> || std::is_same_v<T, std::int64_t> ||
			                       std::is_same_v<T, std::uint64_t> || std::is_same_v<T, float> || std::is_same_v<T, double> ||
			                       std::is_same_v<T, long double>)
			    {
				    string_to_number(value, destination);
			    }
			    else if constexpr (std::is_same_v<T, std::string>)
			    {
				    destination = value;
			    }
			    else if constexpr (std::is_same_v<T, byte_string>)
			    {
				    hex_string_to_binary(value, destination);
			    }
			    else if constexpr (std::is_same_v<T, time_point>)
			    {
				    string_to_time_point(value, destination);
			    }
			    else if constexpr (std::is_same_v<T, date>)
			    {
				    string_to_date(value, destination);
			    }
			    else if constexpr (std::is_same_v<T, time_of_day>)
			    {
				    string_to_time_of_day(value, destination);
			    }
#ifdef SQUID_HAVE_BOOST_DATE_TIME
			    else if constexpr (std::is_same_v<T, boost::posix_time::ptime>)
			    {
				    string_to_boost_ptime(value, destination);
			    }
			    else if constexpr (std::is_same_v<T, boost::gregorian::date>)
			    {
				    string_to_boost_date(value, destination);
			    }
			    else if constexpr (std::is_same_v<T, boost::posix_time::time_duration>)
			    {
				    string_to_boost_time_duration(value, destination);
			    }
#endif
			    else
			    {
				    static_assert(always_false_v<T>, "non-exhaustive visitor!");
			    }
		    }
		    catch (const std::exception& e)
		    {
			    std::ostringstream msg;
			    msg << "Cannot convert the text value " << std::quoted(value) << " of column " << std::quoted(column_name)
			        << " to destination type " << demangled_type_name<T>() << ": " << e.what();
			    throw error{ msg.str() };
		    }
	    },
	    result);
}

void store_result(const result& result, const PGresult& pgresult, int row_index, std::string_view column_name, int column_index)
{
	assert(row_index < PQntuples(&pgresult));
	assert(column_index < PQnfields(&pgresult));
	assert(column_name.data());

	const auto& destination = result.value();

	if (PQgetisnull(&pgresult, row_index, column_index))
	{
		std::visit(
		    [&](auto&& arg) {
			    using T = std::decay_t<decltype(arg)>;
			    if constexpr (std::is_same_v<T, result::non_nullable_type>)
			    {
				    std::ostringstream msg;
				    msg << "Cannot store a NULL value of column " << std::quoted(column_name) << " in a non-optional type";
				    throw error{ msg.str() };
			    }
			    else if constexpr (std::is_same_v<T, result::nullable_type>)
			    {
				    std::visit(
				        [](auto&& arg) {
					        // arg is a (std::optional<X>*)
					        *arg = std::nullopt;
				        },
				        arg);
			    }
			    else
			    {
				    static_assert(always_false_v<T>, "non-exhaustive visitor!");
			    }
		    },
		    destination);
	}
	else
	{
		const auto value = PQgetvalue(&pgresult, row_index, column_index);
		assert(value);

		std::visit(
		    [&](auto&& arg) {
			    using T = std::decay_t<decltype(arg)>;
			    if constexpr (std::is_same_v<T, result::non_nullable_type>)
			    {
				    store_result(arg, column_name, value);
			    }
			    else if constexpr (std::is_same_v<T, result::nullable_type>)
			    {
				    std::visit(
				        [&](auto&& arg) {
					        // arg is a (std::optional<X>*)
					        using T = typename std::decay_t<decltype(*arg)>::value_type;
					        T tmp{};
					        store_result(result::non_nullable_type{ &tmp }, column_name, value);
					        *arg = tmp;
				        },
				        arg);
			    }
			    else
			    {
				    static_assert(always_false_v<T>, "non-exhaustive visitor!");
			    }
		    },
		    destination);
	}
}

} // namespace

struct query_results::column
{
	result           res;
	std::string_view name;
	int              index;

	column(const result& res, std::string_view name, int index)
	    : res{ res }
	    , name{ std::move(name) }
	    , index{ index }
	{
	}
};

query_results::query_results(std::shared_ptr<PGresult> pgresult)
    : pgresult_{ std::move(pgresult) }
    , columns_{}
    , field_count_{}
{
	assert(this->pgresult_);

	const auto column_count = PQnfields(this->pgresult_.get());
	if (column_count < 0)
	{
		throw error{ "PQnfields returned a negative value" };
	}

	this->field_count_ = static_cast<size_t>(column_count);
}

query_results::query_results(std::shared_ptr<PGresult> pgresult, const std::vector<result>& results)
    : query_results{ pgresult }
{
	if (results.size() > this->field_count_)
	{
		throw error{ "Cannot fetch " + std::to_string(results.size()) + " columns from a row with only " +
			         std::to_string(this->field_count_) + " column" + (this->field_count_ == 1 ? "" : "s") };
	}

	this->columns_.reserve(results.size());
	int index = 0;
	for (const auto& result : results)
	{
		const auto column_name = PQfname(pgresult.get(), index);
		if (column_name == nullptr)
		{
			throw error{ "PQfname returned a nullptr" };
		}

		this->columns_.push_back(std::make_unique<column>(result, column_name, index));

		++index;
	}
}

query_results::query_results(std::shared_ptr<PGresult> pgresult, const std::map<std::string, result>& results)
    : query_results{ pgresult }
{
	if (results.size() > this->field_count_)
	{
		throw error{ "Cannot fetch " + std::to_string(results.size()) + " columns from a row with only " +
			         std::to_string(this->field_count_) + " column" + (this->field_count_ == 1 ? "" : "s") };
	}

	std::map<std::string_view, size_t> map{};
	for (size_t index = 0; index < this->field_count_; ++index)
	{
		const auto column_name = PQfname(pgresult.get(), index);
		if (column_name == nullptr)
		{
			throw error{ "PQfname returned a nullptr" };
		}
		map[column_name] = index;
	}

	this->columns_.reserve(results.size());
	for (const auto& result : results)
	{
		auto it = map.find(result.first);
		if (it == map.end())
		{
			throw error{ "Column '" + result.first + "' not found in the result" };
		}

		const auto index       = it->second;
		const auto column_name = it->first;

		this->columns_.push_back(std::make_unique<column>(result.second, column_name, index));
	}
}

query_results::~query_results() noexcept
{
}

size_t query_results::field_count() const
{
	return this->field_count_;
}

std::string query_results::field_name(std::size_t index) const
{
	const auto name = PQfname(this->pgresult_.get(), index);
	if (name == nullptr)
	{
		throw error{ "PQfname returned a null pointer" };
	}

	return name;
}

void query_results::fetch(int row_index)
{
	for (const auto& column : this->columns_)
	{
		store_result(column->res, *this->pgresult_, row_index, column->name, column->index);
	}
}

} // namespace postgresql
} // namespace squid
