//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "queryresults.h"

#include "squid/mysql/error.h"
#include "squid/mysql/detail/conversions.h"

#include "squid/detail/always_false.h"
#include "squid/detail/conversions.h"

#include <sstream>
#include <iomanip>
#include <cstring>
#include <cassert>

#ifdef SQUID_DEBUG_MYSQL
#include <iostream>
#endif

#include <mysql/mysql.h>

namespace squid {
namespace mysql {

class query_results::column
{
	std::string_view          name_;
	unsigned int              index_;
	result                    res_;
	MYSQL_BIND*               bind_;
	MYSQL_STMT*               statement_;
	std::optional<MYSQL_TIME> time_;

	void pre_fetch(const result::non_nullable_type& result)
	{
		this->bind_->is_null = &this->bind_->is_null_value;
		this->bind_->length  = &this->bind_->length_value;
		this->bind_->error   = &this->bind_->error_value;

		std::visit(
		    [&](auto&& arg) {
			    using T = std::decay_t<decltype(*arg)>;
			    if constexpr (std::is_same_v<T, bool>)
			    {
				    this->bind_->buffer_type = MYSQL_TYPE_TINY;
				    this->bind_->buffer      = arg;
			    }
			    else if constexpr (std::is_same_v<T, char>)
			    {
				    this->bind_->buffer_type   = MYSQL_TYPE_STRING;
				    this->bind_->buffer        = arg;
				    this->bind_->buffer_length = 1u;
			    }
			    else if constexpr (std::is_same_v<T, signed char> || std::is_same_v<T, unsigned char>)
			    {
				    this->bind_->buffer_type = MYSQL_TYPE_TINY;
				    this->bind_->buffer      = arg;
				    if constexpr (std::is_unsigned_v<T>)
				    {
					    this->bind_->is_unsigned = true;
				    }
			    }
			    else if constexpr (std::is_same_v<T, std::int16_t> || std::is_same_v<T, std::uint16_t>)
			    {
				    this->bind_->buffer_type = MYSQL_TYPE_SHORT;
				    this->bind_->buffer      = arg;
				    if constexpr (std::is_unsigned_v<T>)
				    {
					    this->bind_->is_unsigned = true;
				    }
			    }
			    else if constexpr (std::is_same_v<T, std::int32_t> || std::is_same_v<T, std::uint32_t>)
			    {
				    this->bind_->buffer_type = MYSQL_TYPE_LONG;
				    this->bind_->buffer      = arg;
				    if constexpr (std::is_unsigned_v<T>)
				    {
					    this->bind_->is_unsigned = true;
				    }
			    }
			    else if constexpr (std::is_same_v<T, std::int64_t> || std::is_same_v<T, std::uint64_t>)
			    {
				    this->bind_->buffer_type = MYSQL_TYPE_LONGLONG;
				    this->bind_->buffer      = arg;
				    if constexpr (std::is_unsigned_v<T>)
				    {
					    this->bind_->is_unsigned = true;
				    }
			    }
			    else if constexpr (std::is_same_v<T, float>)
			    {
				    this->bind_->buffer_type = MYSQL_TYPE_FLOAT;
				    this->bind_->buffer      = arg;
			    }
			    else if constexpr (std::is_same_v<T, double>)
			    {
				    this->bind_->buffer_type = MYSQL_TYPE_DOUBLE;
				    this->bind_->buffer      = arg;
			    }
			    else if constexpr (std::is_same_v<T, long double>)
			    {
				    this->bind_->buffer_type = MYSQL_TYPE_DOUBLE;
				    this->bind_->buffer      = arg;
			    }
			    else if constexpr (std::is_same_v<T, std::string>)
			    {
				    this->bind_->buffer_type   = MYSQL_TYPE_STRING;
				    this->bind_->buffer        = nullptr;
				    this->bind_->buffer_length = 0u;
			    }
			    else if constexpr (std::is_same_v<T, byte_string>)
			    {
				    this->bind_->buffer_type   = MYSQL_TYPE_BLOB;
				    this->bind_->buffer        = nullptr;
				    this->bind_->buffer_length = 0u;
			    }
			    else if constexpr (std::is_same_v<T, time_point>)
			    {
				    this->time_.emplace();
				    this->time_->time_type = MYSQL_TIMESTAMP_DATETIME;

				    this->bind_->buffer_type = MYSQL_TYPE_DATETIME;
				    this->bind_->buffer      = &this->time_.value();
			    }
			    else if constexpr (std::is_same_v<T, date>)
			    {
				    this->time_.emplace();
				    this->time_->time_type = MYSQL_TIMESTAMP_DATE;

				    this->bind_->buffer_type = MYSQL_TYPE_DATE;
				    this->bind_->buffer      = &this->time_.value();
			    }
			    else if constexpr (std::is_same_v<T, time_of_day>)
			    {
				    this->time_.emplace();
				    this->time_->time_type = MYSQL_TIMESTAMP_TIME;

				    this->bind_->buffer_type = MYSQL_TYPE_TIME;
				    this->bind_->buffer      = &this->time_.value();
			    }
#ifdef SQUID_HAVE_BOOST_DATE_TIME
			    else if constexpr (std::is_same_v<T, boost::posix_time::ptime>)
			    {
				    this->time_.emplace();
				    this->time_->time_type = MYSQL_TIMESTAMP_DATETIME;

				    this->bind_->buffer_type = MYSQL_TYPE_DATETIME;
				    this->bind_->buffer      = &this->time_.value();
			    }
			    else if constexpr (std::is_same_v<T, boost::gregorian::date>)
			    {
				    this->time_.emplace();
				    this->time_->time_type = MYSQL_TIMESTAMP_DATE;

				    this->bind_->buffer_type = MYSQL_TYPE_DATE;
				    this->bind_->buffer      = &this->time_.value();
			    }
			    else if constexpr (std::is_same_v<T, boost::posix_time::time_duration>)
			    {
				    this->time_.emplace();
				    this->time_->time_type = MYSQL_TIMESTAMP_TIME;

				    this->bind_->buffer_type = MYSQL_TYPE_TIME;
				    this->bind_->buffer      = &this->time_.value();
			    }
#endif
			    else
			    {
				    static_assert(always_false_v<T>, "non-exhaustive visitor!");
			    }
		    },
		    result);
	}

	void post_verify_length(size_t length)
	{
		if (this->bind_->length_value != length)
		{
			std::ostringstream msg;
			msg << "The length of column " << std::quoted(this->name_) << " should be " << length << " but it is "
			    << this->bind_->length_value;
			throw error{ msg.str() };
		}
	}

	void post_fetch(const result::non_nullable_type& result)
	{
		std::visit(
		    [&](auto&& arg) {
			    using T = std::decay_t<decltype(*arg)>;
			    if constexpr (std::is_same_v<T, bool>)
			    {
				    post_verify_length(1u);
			    }
			    else if constexpr (std::is_same_v<T, char>)
			    {
				    if (this->bind_->error_value || this->bind_->length_value != 1u)
				    {
					    std::ostringstream msg;
					    msg << "Cannot store a string of length " << this->bind_->length_value << " of column " << std::quoted(this->name_)
					        << " into a single char";
					    throw error{ msg.str() };
				    }
			    }
			    else if constexpr (std::is_same_v<T, signed char> || std::is_same_v<T, unsigned char>)
			    {
				    post_verify_length(1u);
			    }
			    else if constexpr (std::is_same_v<T, std::int16_t> || std::is_same_v<T, std::uint16_t>)
			    {
				    post_verify_length(2u);
			    }
			    else if constexpr (std::is_same_v<T, std::int32_t> || std::is_same_v<T, std::uint32_t>)
			    {
				    post_verify_length(4u);
			    }
			    else if constexpr (std::is_same_v<T, std::int64_t> || std::is_same_v<T, std::uint64_t>)
			    {
				    post_verify_length(8u);
			    }
			    else if constexpr (std::is_same_v<T, float>)
			    {
				    post_verify_length(4u);
			    }
			    else if constexpr (std::is_same_v<T, double>)
			    {
				    post_verify_length(8u);
			    }
			    else if constexpr (std::is_same_v<T, long double>)
			    {
				    post_verify_length(8u);
				    *arg = static_cast<long double>(*reinterpret_cast<double*>(this->bind_->buffer));
			    }
			    else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, byte_string>)
			    {
				    arg->resize(this->bind_->length_value);
				    if (this->bind_->length_value > 0ul)
				    {
					    this->bind_->buffer        = arg->data();
					    this->bind_->buffer_length = this->bind_->length_value;
					    if (0 != mysql_stmt_fetch_column(this->statement_, this->bind_, this->index_, 0ul))
					    {
						    throw error{ "mysql_stmt_fetch_column failed", *this->statement_ };
					    }
				    }
			    }
			    else if constexpr (std::is_same_v<T, time_point> || std::is_same_v<T, date> || std::is_same_v<T, time_of_day>
#ifdef SQUID_HAVE_BOOST_DATE_TIME
			                       || std::is_same_v<T, boost::posix_time::ptime> || std::is_same_v<T, boost::gregorian::date> ||
			                       std::is_same_v<T, boost::posix_time::time_duration>
#endif
			    )
			    {
				    assert(this->time_.has_value());
				    if (this->name_ == "u")
				    {
					    std::cout << "*** " << this->name_ << " ***" << std::endl;
				    }
				    from_mysql_time(this->time_.value(), *arg);
			    }
			    else
			    {
				    static_assert(always_false_v<T>, "non-exhaustive visitor!");
			    }
		    },
		    result);
	}

public:
	column(std::string_view name, unsigned int index, const result& res, MYSQL_BIND* bind, MYSQL_STMT* statement)
	    : name_{ name }
	    , index_{ index }
	    , res_{ res }
	    , bind_{ bind }
	    , statement_{ statement }
	    , time_{}
	{
		assert(bind);
		assert(statement);

		std::visit(
		    [&](auto&& arg) {
			    using T = std::decay_t<decltype(arg)>;
			    if constexpr (std::is_same_v<T, result::non_nullable_type>)
			    {
				    this->pre_fetch(arg);
			    }
			    else if constexpr (std::is_same_v<T, result::nullable_type>)
			    {
				    std::visit(
				        [&](auto&& arg) {
					        // arg is a (std::optional<X>*)
					        arg->emplace();
					        using T = typename std::decay_t<decltype(*arg)>::value_type;
					        T* tmp{ &arg->value() };
					        this->pre_fetch(result::non_nullable_type{ tmp });
				        },
				        arg);
			    }
			    else
			    {
				    static_assert(always_false_v<T>, "non-exhaustive visitor!");
			    }
		    },
		    res.value());
	}

	void post_fetch()
	{
		std::visit(
		    [&](auto&& arg) {
			    using T = std::decay_t<decltype(arg)>;
			    if constexpr (std::is_same_v<T, result::non_nullable_type>)
			    {
				    if (this->bind_->is_null_value)
				    {
					    std::ostringstream msg;
					    msg << "Cannot store a NULL value of column " << std::quoted(this->name_) << " in a non-optional type";
					    throw error{ msg.str() };
				    }
				    else
				    {
					    this->post_fetch(arg);
				    }
			    }
			    else if constexpr (std::is_same_v<T, result::nullable_type>)
			    {
				    std::visit(
				        [&](auto&& arg) {
					        // arg is a (std::optional<X>*)
					        // *arg was emplaced in the c'tor
					        assert(arg->has_value());
					        if (this->bind_->is_null_value)
					        {
						        arg->reset();
					        }
					        else
					        {
						        using T = typename std::decay_t<decltype(*arg)>::value_type;
						        T* tmp{ &arg->value() };
						        this->post_fetch(result::non_nullable_type{ tmp });
					        }
				        },
				        arg);
			    }
			    else
			    {
				    static_assert(always_false_v<T>, "non-exhaustive visitor!");
			    }
		    },
		    this->res_.value());
	}
};

query_results::query_results(std::shared_ptr<MYSQL_STMT> statement)
    : statement_{ statement }
    , meta_{ mysql_stmt_result_metadata(statement.get()), mysql_free_result }
    , fields_{}
    , field_count_{}
    , binds_{}
    , columns_{}
{
	assert(statement);

	if (this->meta_)
	{
		this->fields_ = mysql_fetch_fields(this->meta_.get());
		if (!this->fields_)
		{
			throw error{ "mysql_fetch_fields returned a nullptr", *this->statement_ };
		}

		this->field_count_ = static_cast<std::size_t>(mysql_num_fields(this->meta_.get()));

		this->binds_.resize(this->field_count_);

		for (auto& bind : this->binds_)
		{
			std::memset(&bind, 0, sizeof(bind));
			bind.buffer_type = MYSQL_TYPE_NULL;
		}
	}
}

query_results::query_results(std::shared_ptr<MYSQL_STMT> statement, const std::vector<result>& results)
    : query_results{ statement }
{
	if (results.size() > this->field_count_)
	{
		throw error{ "Cannot fetch " + std::to_string(results.size()) + " columns from a result set with " +
			         std::to_string(this->field_count_) + " column" + (this->field_count_ == 1 ? "" : "s") };
	}

	if (0u == this->field_count_)
	{
		return;
	}

	this->columns_.reserve(results.size());

	for (std::size_t i = 0, end = results.size(); i < end; ++i)
	{
		assert(i < this->binds_.size());
		this->columns_.push_back(std::make_unique<column>(this->field_name(i), i, results[i], &this->binds_[i], statement.get()));
	}

	if (mysql_stmt_bind_result(this->statement_.get(), &this->binds_.front()))
	{
		throw error{ "mysql_stmt_bind_result failed", *this->statement_ };
	}
}

query_results::query_results(std::shared_ptr<MYSQL_STMT> statement, const std::map<std::string, result>& results)
    : query_results{ statement }
{
	if (results.size() > this->field_count_)
	{
		throw error{ "Cannot fetch " + std::to_string(results.size()) + " columns from a result set with " +
			         std::to_string(this->field_count_) + " column" + (this->field_count_ == 1 ? "" : "s") };
	}

	if (0u == this->field_count_)
	{
		return;
	}

	std::map<std::string_view, std::size_t> name_map{};
	for (std::size_t i = 0, end = this->field_count_; i < end; ++i)
	{
		assert(this->fields_);
		const auto name = this->fields_[i].name;
		if (name == nullptr)
		{
			throw error{ "mysql_fetch_fields returned a field with a null pointer for a name" };
		}
		name_map[std::string_view{ name, this->fields_[i].name_length }] = i;
	}

	this->columns_.reserve(results.size());

	for (const auto& result : results)
	{
		auto it = name_map.find(result.first);
		if (it == name_map.end())
		{
			throw error{ "Column '" + result.first + "' not found in the result" };
		}

		assert(it->second < this->binds_.size());
		this->columns_.push_back(
		    std::make_unique<column>(result.first, it->second, result.second, &this->binds_[it->second], statement.get()));
	}

	if (0 != mysql_stmt_bind_result(this->statement_.get(), &this->binds_.front()))
	{
		throw error{ "mysql_stmt_bind_result failed", *this->statement_ };
	}
}

query_results::~query_results() noexcept
{
	mysql_stmt_free_result(this->statement_.get());
}

size_t query_results::field_count() const
{
	return this->field_count_;
}

std::string_view query_results::field_name(std::size_t index) const
{
	if (index < this->field_count_)
	{
		assert(this->fields_);
		const auto name = this->fields_[index].name;
		if (name == nullptr)
		{
			throw error{ "mysql_fetch_fields returned a field with a null pointer for a name" };
		}
		else
		{
			return std::string_view{ name, this->fields_[index].name_length };
		}
	}
	else
	{
		throw error{ "Field index out of bounds" };
	}
}

bool query_results::fetch()
{
	switch (mysql_stmt_fetch(this->statement_.get()))
	{
	case 0:
	case MYSQL_DATA_TRUNCATED:
		break;
	case MYSQL_NO_DATA:
		return false;
	default:
		throw error{ "mysql_stmt_fetch failed", *this->statement_ };
	}

	for (const auto& column : this->columns_)
	{
		column->post_fetch();
	}

	return true;
}

} // namespace mysql
} // namespace squid
