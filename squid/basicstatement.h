//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "api.h"
#include "parameter.h"
#include "result.h"
#include "error.h"

#include "detail/is_scoped_enum.h"

#include <map>
#include <vector>
#include <memory>
#include <string_view>
#include <string>

namespace squid {

class IBackendStatement;

class SQUID_API BasicStatement
{
	std::map<std::string, Parameter>   parameters_;
	std::vector<Result>                results_;
	std::unique_ptr<IBackendStatement> statement_;

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

	template<typename T>
	BasicStatement& bind(std::string_view name, const T& value)
	{
		if constexpr (std::is_enum_v<T>)
		{
			if constexpr (std::is_scoped_enum_v<T>)
			{
				using base = std::underlying_type_t<T>;
				if constexpr (std::is_same_v<base, char>)
				{
					const auto c = static_cast<char>(value);
					if (c < 0x20 || c > 0x7f)
					{
						throw Error{ "Binding a scoped enum with char as underlying type requires enum values within range [0x20 - 0x7F]" };
					}
					this->upsertParameter(name, std::string{ 1, c });
				}
				else
				{
					this->upsertParameter(name, static_cast<base>(value));
				}
			}
			else
			{
				this->upsertParameter(name, static_cast<int>(value));
			}
		}
		else
		{
			this->upsertParameter(name, value);
		}
		return *this;
	}

	template<typename T>
	BasicStatement& bind(std::string_view name, const std::optional<T>& value)
	{
		if (value.has_value())
		{
			this->bind(name, value.value());
		}
		else
		{
			this->upsertParameter(name, std::nullopt);
		}
		return *this;
	}

	BasicStatement& bind(std::string_view name, const unsigned char* value, std::size_t size);

	template<typename T>
	BasicStatement& bindResult(T& value)
	{
		this->results_.emplace_back(value);
		return *this;
	}

	void execute();

	bool fetch();
};

} // namespace squid
