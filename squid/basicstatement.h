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

#ifdef SQUID_HAVE_BOOST_SERIALIZATION
#include "squid/detail/bind_oarchive.h"
#include "squid/detail/bind_iarchive.h"
#endif

#include <map>
#include <vector>
#include <memory>
#include <string_view>
#include <string>

namespace squid {

class IBackendStatement;

/// Base class for Statement and PreparedStatement
/// Not intended to be instantiated directly.
class SQUID_EXPORT BasicStatement
{
	std::map<std::string, Parameter>   parameters_;   /// bound query parameter
	std::vector<Result>                results_;      /// bound row results, by sequence
	std::map<std::string, Result>      namedResults_; /// bound row results, by name
	std::unique_ptr<IBackendStatement> statement_;    /// backend statement

	template<typename... Args>
	void upsertParameter(std::string_view name, Args&&... args)
	{
		this->parameters_.insert_or_assign(std::string{ name }, Parameter{ std::forward<Args>(args)... });
	}

	template<class... Results>
	BasicStatement& bindResultsImpl(Results&&... refs)
	{
		std::vector<Result> results{ refs... };
		this->results_.swap(results);
		return *this;
	}

public:
	explicit BasicStatement(std::unique_ptr<IBackendStatement>&& statement);

	virtual ~BasicStatement() noexcept;

	BasicStatement(const BasicStatement&)            = delete;
	BasicStatement(BasicStatement&& src)             = default;
	BasicStatement& operator=(const BasicStatement&) = delete;
	BasicStatement& operator=(BasicStatement&&)      = default;

	/// Parameter binding methods
	/// See also parameter.h for the supported types.

	/// Bind a query parameter @a name with @a value.
	/// The value is copied, but note that for view types (std::string_view and byte_string_view)
	/// the value is not deep copied. For these types the data pointed to by the view must outlive
	/// the statement.
	/// The bind will override a previous parameter bind with the same name.
	template<typename T>
	BasicStatement& bind(std::string_view name, const T& value)
	{
		this->upsertParameter(name, value, Parameter::ByValue{});
		return *this;
	}

	/// Bind a query parameter @a name with a binary array @a value of length @a size.
	/// The value is not copied, so it must outlive the statement.
	/// To have the value copied, use a byte_string instead.
	/// The bind will override a previous parameter bind with the same name.
	BasicStatement& bind(std::string_view name, const unsigned char* value, std::size_t size);

	/// Bind the query parameter(s) from the members of a struct or class T @a value.
	/// T must have a public method template<class Binder> void bind(Binder& b).
	/// Assuming T has 2 members foo and bar, then this method should call b.bind("foo", foo); and
	/// b.bind("bar", bar); to have those 2 members bound with their respective names.
	/// The values are copied, but note that for view types (std::string_view and byte_string_view)
	/// the values are not deep copied. For these types the data pointed to by the view must outlive
	/// the statement.
	template<typename T>
	std::enable_if_t<has_bind_method<T, ParameterBinder<BasicStatement>>, BasicStatement&> bind(const T& value)
	{
		ParameterBinder<BasicStatement> binder{ *this };
		const_cast<T&>(value).bind(binder); // not to worry, value is not modified.
		return *this;
	}

#ifdef SQUID_HAVE_BOOST_SERIALIZATION
	/// Bind the query parameter(s) from the members of a struct or class T @a value.
	/// T must be boost serializable.
	/// The values are copied, but note that for view types (std::string_view and byte_string_view)
	/// the values are not deep copied. For these types the data pointed to by the view must outlive
	/// the statement.
	/// If T has a bind method (see concept has_bind_method) then that implementation takes precedence.
	template<typename T>
	std::enable_if_t<!has_bind_method<T, ParameterBinder<BasicStatement>>, BasicStatement&> bind(const T& value)
	{
		bind_oarchive<ParameterBinder<BasicStatement>> ar{ *this };
		ar << value;
		return *this;
	}
#endif

	/// Bind a query parameter @a name with @a value by reference.
	/// The reference must outlive the statement.
	/// The bind will override a previous parameter bind with the same name.
	template<typename T>
	BasicStatement& bindRef(std::string_view name, const T& value)
	{
		this->upsertParameter(name, value, Parameter::ByReference{});
		return *this;
	}

	/// Bind the query parameter(s) from the members of a struct or class T @a value by reference.
	/// The reference must outlive the statement.
	template<typename T>
	std::enable_if_t<has_bind_method<T, ParameterRefBinder<BasicStatement>>, BasicStatement&> bindRef(const T& value)
	{
		ParameterRefBinder<BasicStatement> binder{ *this };
		const_cast<T&>(value).bind(binder); // not to worry, value is not modified.
		return *this;
	}

#ifdef SQUID_HAVE_BOOST_SERIALIZATION
	/// Bind the query parameter(s) from the members of a struct or class T @a value by reference.
	/// T must be boost serializable.
	/// The reference must outlive the statement.
	/// If T has a bind method (see concept has_bind_method) then that implementation takes precedence.
	template<typename T>
	std::enable_if_t<!has_bind_method<T, ParameterBinder<BasicStatement>>, BasicStatement&> bindRef(const T& value)
	{
		bind_oarchive<ParameterRefBinder<BasicStatement>> ar{ *this };
		ar << value;
		return *this;
	}
#endif

	/// Result binding methods
	/// See also result.h for the supported types.

	/// Bind the next row result column to @a ref.
	/// The first call binds the first result column, the next call binds the second column, and so on.
	/// This sequential result binding cannot be combined with result binding by name.
	template<typename T>
	BasicStatement& bindResult(T& ref)
	{
		this->results_.emplace_back(ref);
		return *this;
	}

	/// Bind the row result column with name @a name to @a ref.
	/// This named result binding cannot be combined with sequential result binding.
	/// The bind will override a previous result bind with the same name.
	template<typename T>
	BasicStatement& bindResult(std::string_view name, T& ref)
	{
		this->namedResults_.insert_or_assign(std::string{ name }, Result{ ref });
		return *this;
	}

	/// Bind the row result to the given @a refs.
	/// The order of the arguments must match the order of the query result columns.
	/// This clears all previous result bindings made by either bindResult or bindResults.
	template<class... Results>
	BasicStatement& bindResults(Results&... refs)
	{
		return this->bindResultsImpl(Result(refs)...);
	}

	/// Bind the row result to the members of a struct or class T @a ref.
	/// T must have a public method template<class Binder> void bind(Binder& b).
	/// Assuming T has 2 members foo and bar, then this method should call b.bind("foo", foo); and
	/// b.bind("bar", bar); to have those 2 members bound with their respective names.
	template<typename T>
	std::enable_if_t<has_bind_method<T, ResultBinder<BasicStatement>>, BasicStatement&> bindResults(T& ref)
	{
		ResultBinder<BasicStatement> binder{ *this };
		ref.bind(binder);
		return *this;
	}

#ifdef SQUID_HAVE_BOOST_SERIALIZATION
	/// Bind the row result to the members of a struct or class T @a ref.
	/// T must be boost serializable.
	/// The reference must outlive the statement.
	/// If T has a bind method (see concept has_bind_method) then that implementation takes precedence.
	template<typename T>
	std::enable_if_t<!has_bind_method<T, ResultBinder<BasicStatement>>, BasicStatement&> bindResults(T& ref)
	{
		bind_iarchive<ResultBinder<BasicStatement>> ar{ *this };
		ar >> ref;
		return *this;
	}
#endif

	/// Statement execution methods

	/// Execute the statement.
	void execute();

	/// Fetch the next row.
	/// Returns false when the last row was already fetched or when the statement
	/// did not return any rows.
	bool fetch();

	/// Get the number of fields in the result set.
	std::size_t getFieldCount();

	/// Get the name of index'th field in the result set.
	/// The first field has index 0.
	std::string getFieldName(std::size_t index);
};

} // namespace squid
