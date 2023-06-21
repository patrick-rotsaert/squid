//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/mysql/backendconnection.h"
#include "squid/mysql/statement.h"
#include "squid/mysql/error.h"

#include <mysql/mysql.h>

#include <mutex>
#include <optional>
#include <string>
#include <cctype>
#include <cstdlib>

namespace squid {
namespace mysql {

namespace {

struct mutex_singleton
{
	std::mutex mutex;

	static mutex_singleton& instance()
	{
		static mutex_singleton _;
		return _;
	}
};

constexpr auto WHITESPACE = " \t\r\n";

void skipws(std::string_view& in)
{
	const auto pos = in.find_first_not_of(WHITESPACE);
	if (pos == std::string_view::npos)
	{
		in = in.substr(in.length());
	}
	else
	{
		in = in.substr(pos);
	}
}

void skipn(std::string_view& in, std::string_view::size_type n)
{
	in = in.substr(n);
}

const auto ERROR_MESSAGE_MALFORMED = std::string{ "Malformed connection string" };

std::string_view parse_parameter_name(std::string_view& in)
{
	std::string_view::size_type pos = 0u;
	for (const auto end = in.length(); pos < end; ++pos)
	{
		if (!std::isalpha(in.at(pos)) && in.at(pos) != '_')
		{
			break;
		}
	}
	const auto identifier = in.substr(0, pos);
	skipn(in, pos);
	return identifier;
}

std::string parse_parameter_value(std::string_view& in)
{
	if (in.empty())
	{
		return std::string{};
	}
	else if (in.front() == '"')
	{
		skipn(in, 1u);
		std::string value{};
		bool        escape = false;
		for (;;)
		{
			if (in.empty())
			{
				throw error{ ERROR_MESSAGE_MALFORMED + ": unterminated quoted value" };
			}
			const auto c = in.front();
			skipn(in, 1u);
			if (c == '"')
			{
				if (escape)
				{
					value.append({ c });
					escape = false;
				}
				else
				{
					break;
				}
			}
			else if (c == '\\')
			{
				if (escape)
				{
					value.append({ c });
					escape = false;
				}
				else
				{
					escape = true;
				}
			}
			else if (escape)
			{
				throw error{ ERROR_MESSAGE_MALFORMED + ": only `\"' and `\\' characters may be escaped" };
			}
			else
			{
				value.append({ c });
			}
		}

		return value;
	}
	else
	{
		const auto pos = in.find_first_of(WHITESPACE);
		if (pos == std::string_view::npos)
		{
			const auto value = in;
			skipn(in, in.length());
			return std::string{ value };
		}
		else
		{
			const auto value = in.substr(0, pos);
			skipn(in, pos);
			return std::string{ value };
		}
	}
}

struct connection_parameters
{
	std::optional<std::string>  host;
	std::optional<std::string>  user;
	std::optional<std::string>  passwd;
	std::optional<std::string>  db;
	std::optional<std::string>  unix_socket;
	std::optional<unsigned int> port;
	std::optional<std::string>  sslca;
	std::optional<std::string>  sslcert;
	std::optional<std::string>  sslkey;
	std::optional<std::string>  charset;
};

connection_parameters parse_connection_string(std::string_view in)
{
	connection_parameters params;

	for (;;)
	{
		skipws(in);

		if (in.empty())
		{
			break;
		}

		const auto name = parse_parameter_name(in);
		if (name.empty())
		{
			throw error{ ERROR_MESSAGE_MALFORMED + ": parameter name expected" };
		}

		skipws(in);

		if (in.empty() || in.front() != '=')
		{
			throw error{ ERROR_MESSAGE_MALFORMED + ": '=' expected after parameter name '" + std::string{ name } + "'" };
		}

		skipn(in, 1u);

		skipws(in);

		const auto value = parse_parameter_value(in);

		if (name == "host")
		{
			params.host = value;
		}
		else if (name == "user")
		{
			params.user = value;
		}
		else if (name == "passwd")
		{
			params.passwd = value;
		}
		else if (name == "db")
		{
			params.db = value;
		}
		else if (name == "unix_socket")
		{
			params.unix_socket = value;
		}
		else if (name == "port")
		{
			char*      end  = nullptr;
			const auto port = strtoul(value.c_str(), &end, 0);
			if (!end || *end)
			{
				throw error{ "Invalid connection string. Value for `port' parameter is not a valid integer" };
			}
			params.port = static_cast<unsigned int>(port);
		}
		else if (name == "sslca")
		{
			params.sslca = value;
		}
		else if (name == "sslcert")
		{
			params.sslcert = value;
		}
		else if (name == "sslkey")
		{
			params.sslkey = value;
		}
		else if (name == "charset")
		{
			params.charset = value;
		}
		else
		{
			throw error{ "Invalid connection string. Unknown parameter name '" + std::string{ name } + "'" };
		}
	}

	return params;
}

std::shared_ptr<MYSQL> connect_database(std::string_view connection_info)
{
	std::unique_lock<std::mutex> lock{ mutex_singleton::instance().mutex };
	std::shared_ptr<MYSQL>       handle{ mysql_init(nullptr), mysql_close };
	lock.unlock();

	if (!handle)
	{
		throw error{ "mysql_init: out of memory" };
	}

	const auto params = parse_connection_string(connection_info);

	if (params.charset)
	{
		if (mysql_options(handle.get(), MYSQL_SET_CHARSET_NAME, params.charset->c_str()))
		{
			throw error("mysql_options(MYSQL_SET_CHARSET_NAME) failed", *handle);
		}
	}

	if (params.sslca)
	{
		mysql_ssl_set(handle.get(),
		              params.sslkey ? params.sslkey->c_str() : nullptr,
		              params.sslcert ? params.sslcert->c_str() : nullptr,
		              params.sslca ? params.sslca->c_str() : nullptr,
		              0,
		              0);
	}

	if (mysql_real_connect(handle.get(),
	                       params.host ? params.host->c_str() : nullptr,
	                       params.user ? params.user->c_str() : nullptr,
	                       params.passwd ? params.passwd->c_str() : nullptr,
	                       params.db ? params.db->c_str() : nullptr,
	                       params.port.value_or(0),
	                       params.unix_socket ? params.unix_socket->c_str() : nullptr,
	                       CLIENT_FOUND_ROWS) == nullptr)
	{
		throw error("mysql_real_connect failed", *handle);
	}

	return handle;
}

} // namespace

// All statements are prepared statements.

std::unique_ptr<ibackend_statement> backend_connection::create_statement(std::string_view query)
{
	return std::make_unique<statement>(this->connection_, query, false);
}

std::unique_ptr<ibackend_statement> backend_connection::create_prepared_statement(std::string_view query)
{
	return std::make_unique<statement>(this->connection_, query, true);
}

void backend_connection::execute(const std::string& query)
{
	statement::execute(*this->connection_, query);
}

backend_connection::backend_connection(const std::string& connection_info)
    : connection_{ connect_database(connection_info) }
{
}

MYSQL& backend_connection::handle() const
{
	return *this->connection_;
}

} // namespace mysql
} // namespace squid
