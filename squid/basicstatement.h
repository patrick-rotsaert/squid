//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/parameter.h"
#include "squid/result.h"
#include "squid/error.h"

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
	std::map<std::string, Parameter>   parameters_; /// bound query parameter
	std::vector<Result>                results_;    /// bound row results
	std::unique_ptr<IBackendStatement> statement_;  /// backend statement

	template<typename... Args>
	void upsertParameter(std::string_view name, Args&&... args)
	{
		this->parameters_.insert_or_assign(std::string{ name }, Parameter{ std::forward<Args>(args)... });
	}

public:
	explicit BasicStatement(std::unique_ptr<IBackendStatement>&& statement);

	virtual ~BasicStatement() noexcept;

	BasicStatement(const BasicStatement&) = delete;
	BasicStatement(BasicStatement&& src)  = default;
	BasicStatement& operator=(const BasicStatement&) = delete;
	BasicStatement& operator=(BasicStatement&&) = default;

	/// Parameter binding methods
	/// See also parameter.h for the supported types.

	/// Bind a query parameter @a name with @a value by value.
	/// The value is copied, but note that for view types (std::string_view and byte_string_view)
	/// the value is not deep copied. For these types the data pointed to by the view must outlive
	/// the statement.
	template<typename T>
	BasicStatement& bind(std::string_view name, const T& value)
	{
		this->upsertParameter(name, value, Parameter::ByValue{});
		return *this;
	}

	/// Bind a query parameter @a name with a binary array @a value of length @a size.
	/// The value is not copied, so it must outlive the statement.
	/// To have the value copied, use a byte_string instead.
	BasicStatement& bind(std::string_view name, const unsigned char* value, std::size_t size);

	/// Bind a query parameter @a name with @a value by reference.
	/// The reference must outlive the statement.
	template<typename T>
	BasicStatement& bindRef(std::string_view name, const T& value)
	{
		this->upsertParameter(name, value, Parameter::ByReference{});
		return *this;
	}

	/// Bind the next row result column to @a value.
	/// The first call binds the first result column, the next call binds the second column, etc...
	/// See also result.h for the supported types.
	template<typename T>
	BasicStatement& bindResult(T& value)
	{
		this->results_.emplace_back(value);
		return *this;
	}

	/// Execute the statement.
	void execute();

	/// Fetch the next row.
	/// Returns false when the last row was already fetched or when the statement
	/// did not return any rows.
	bool fetch();
};

} // namespace squid
