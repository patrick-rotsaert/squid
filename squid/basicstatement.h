//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/parameter.h"
#include "squid/result.h"
#include "squid/error.h"
#include "squid/config.h"

#include "squid/detail/parameterbinder.h"
#include "squid/detail/resultbinder.h"
#include "squid/detail/type_traits.h"
#include "squid/detail/always_false.h"

#ifdef SQUID_HAVE_BOOST_SERIALIZATION
#include "squid/detail/bind_oarchive.h"
#include "squid/detail/bind_iarchive.h"
#endif

#include <map>
#include <vector>
#include <memory>
#include <string_view>
#include <string>
#include <optional>
#include <sstream>

namespace squid {

class ibackend_connection;
class ibackend_statement;

/// Base class for statement and prepared_statement
/// Not intended to be instantiated directly.
class SQUID_EXPORT basic_statement
{
	std::map<std::string, parameter>     parameters_;    /// bound query parameters
	std::vector<result>                  results_;       /// bound row results, by sequence
	std::map<std::string, result>        named_results_; /// bound row results, by name
	std::shared_ptr<ibackend_connection> connection_;    /// backend connection
	std::unique_ptr<ibackend_statement>  statement_;     /// backend statement
	std::optional<std::ostringstream>    query_;         /// query stream

	template<typename... Args>
	void upsert_parameter(std::string_view name, Args&&... args)
	{
		this->parameters_.insert_or_assign(std::string{ name }, parameter{ std::forward<Args>(args)... });
	}

	virtual std::unique_ptr<ibackend_statement> create_statement(std::shared_ptr<ibackend_connection> connection,
	                                                             std::string_view                     query) = 0;

public:
	explicit basic_statement(std::shared_ptr<ibackend_connection> connection, std::unique_ptr<ibackend_statement>&& statement);
	explicit basic_statement(std::shared_ptr<ibackend_connection> connection);

	virtual ~basic_statement() noexcept;

	basic_statement(const basic_statement&)            = delete;
	basic_statement(basic_statement&& src)             = default;
	basic_statement& operator=(const basic_statement&) = delete;
	basic_statement& operator=(basic_statement&&)      = default;

	///
	/// Query modifier methods
	/// These methods reset any previous backend statement

	std::ostream& query();

	template<typename T>
	basic_statement& operator<<(const T& rhs)
	{
		this->query() << rhs;
		return *this;
	}

	///
	/// parameter binding methods
	/// See also parameter.h for the supported types.

	/// Bind a query parameter @a name with @a value.
	/// The value is copied, but note that for view types (std::string_view and byte_string_view)
	/// the value is not deep copied. For these types the data pointed to by the view must outlive
	/// the statement.
	/// The bind will override a previous parameter bind with the same name.
	template<typename T>
	basic_statement& bind(std::string_view name, const T& value)
	{
		this->upsert_parameter(name, value, parameter::by_value{});
		return *this;
	}

	/// Bind a query parameter @a name with a binary array @a value of length @a size.
	/// The value is not copied, so it must outlive the statement.
	/// To have the value copied, use a byte_string instead.
	/// The bind will override a previous parameter bind with the same name.
	basic_statement& bind(std::string_view name, const unsigned char* value, std::size_t size);

	/// Bind the query parameter(s) from the members of a struct or class T @a value.
	/// T must be Boost serializable or T must have a public method template<class Binder> void bind(Binder& b):
	///   assuming T has 2 members foo and bar, then this method should call b.bind("foo", foo); and
	///   b.bind("bar", bar); to have those 2 members bound with their respective names.
	/// If T has a bind method (see concept has_bind_method) then that implementation takes precedence.
	/// The values are copied, but note that for view types (std::string_view and byte_string_view)
	/// the values are not deep copied. For these types the data pointed to by the view must outlive
	/// the statement.
	template<typename T>
	basic_statement& bind(const T& value)
	{
		if constexpr (has_bind_method<T, parameter_binder<basic_statement>>)
		{
			parameter_binder<basic_statement> binder{ *this };
			const_cast<T&>(value).bind(binder); // not to worry, value is not modified.
		}
#ifdef SQUID_HAVE_BOOST_SERIALIZATION
		else if constexpr (is_boost_serializable_v<T, bind_oarchive<parameter_binder<basic_statement>>>)
		{
			bind_oarchive<parameter_binder<basic_statement>> ar{ *this };
			ar << value;
		}
#endif
		else
		{
			static_assert(always_false_v<T>, "Only serializable types allowed");
		}
		return *this;
	}

	/// Bind a query parameter @a name with @a value by reference.
	/// The reference must outlive the statement.
	/// The bind will override a previous parameter bind with the same name.
	template<typename T>
	basic_statement& bind_ref(std::string_view name, const T& value)
	{
		this->upsert_parameter(name, value, parameter::by_reference{});
		return *this;
	}

	/// Bind the query parameter(s) from the members of a struct or class T @a value by reference.
	/// The reference must outlive the statement.
	/// T must be Boost serializable or T must have a public method template<class Binder> void bind(Binder& b).
	/// If T has a bind method (see concept has_bind_method) then that implementation takes precedence.
	template<typename T>
	basic_statement& bind_ref(const T& value)
	{
		if constexpr (has_bind_method<T, parameter_ref_binder<basic_statement>>)
		{
			parameter_ref_binder<basic_statement> binder{ *this };
			const_cast<T&>(value).bind(binder); // not to worry, value is not modified.
		}
#ifdef SQUID_HAVE_BOOST_SERIALIZATION
		else if constexpr (is_boost_serializable_v<T, bind_oarchive<parameter_ref_binder<basic_statement>>>)
		{
			bind_oarchive<parameter_ref_binder<basic_statement>> ar{ *this };
			ar << value;
			return *this;
		}
#endif
		else
		{
			static_assert(always_false_v<T>, "Only serializable types allowed");
		}
		return *this;
	}

	///
	/// result binding methods
	/// See also result.h for the supported types.

	/// Bind the next row result column to @a ref.
	/// The first call binds the first result column, the next call binds the second column, and so on.
	/// This sequential result binding cannot be combined with result binding by name.
	template<typename T>
	basic_statement& bind_result(T& ref)
	{
		this->results_.emplace_back(ref);
		return *this;
	}

	/// Bind the row result column with name @a name to @a ref.
	/// This named result binding cannot be combined with sequential result binding.
	/// The bind will override a previous result bind with the same name.
	template<typename T>
	basic_statement& bind_result(std::string_view name, T& ref)
	{
		this->named_results_.insert_or_assign(std::string{ name }, result{ ref });
		return *this;
	}

	/// Bind the next row result column(s) to the given @a first, rest...
	/// This can be used for either named (bindable structs) or unnamed references, but not both.
	/// Bindable structs must be Boost serializable or must have a public method template<class Binder> void bind(Binder& b):
	///   assuming the struct has 2 members foo and bar, then this method should call b.bind("foo", foo); and
	///   b.bind("bar", bar); to have those 2 members bound with their respective names.
	///   If the struct has a bind method (see concept has_bind_method) then that implementation takes precedence over
	///   Boost serialize.
	/// For unnamed references, the order of the arguments must match the order of the query result columns.
	/// Unnamed references are appended to previously bound references.
	/// Named references will override previous references with the samed name.
	template<class T, class... Ts>
	basic_statement& bind_results(T& first, Ts&... rest)
	{
		if constexpr (has_bind_method<T, result_binder<basic_statement>>)
		{
			result_binder<basic_statement> binder{ *this };
			first.bind(binder);
			return *this;
		}
#ifdef SQUID_HAVE_BOOST_SERIALIZATION
		else if constexpr (is_boost_serializable_v<T, bind_iarchive<result_binder<basic_statement>>>)
		{
			bind_iarchive<result_binder<basic_statement>> ar{ *this };
			ar >> first;
			return *this;
		}
#endif
		else
		{
			this->results_.emplace_back(first);
		}

		if constexpr (sizeof...(rest) > 0)
		{
			return bind_results(rest...);
		}
		else
		{
			return *this;
		}
	}

	///
	/// statement execution methods

	/// Execute the statement.
	void execute();

	/// Fetch the next row.
	/// Returns false when the last row was already fetched or when the statement
	/// did not return any rows.
	/// Throws if the statement has not been executed.
	bool fetch();

	/// Get the number of fields in the result set.
	/// Throws if the statement has not been executed.
	std::size_t field_count();

	/// Get the name of index'th field in the result set.
	/// The first field has index 0.
	/// Throws if the statement has not been executed.
	std::string field_name(std::size_t index);

	/// Get the number of affected rows by the statement.
	/// Only useful for command statements.
	/// Throws if the statement has not been executed.
	std::uint64_t affected_rows();

	ibackend_statement& backend_statement() const;
};

} // namespace squid
