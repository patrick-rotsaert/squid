//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/mysql/statement.h"
#include "squid/mysql/error.h"

#include "squid/mysql/detail/query.h"
#include "squid/mysql/detail/queryparameters.h"
#include "squid/mysql/detail/queryresults.h"

#include <sstream>
#include <iomanip>
#include <cassert>
#include <cstring>
#include <vector>

#ifdef SQUID_DEBUG_MYSQL
#include <iostream>
#endif

#include <mysql/mysql.h>

namespace squid {
namespace mysql {

namespace {

std::shared_ptr<MYSQL_STMT> prepare_statement(MYSQL& connection, const std::string& query)
{
#ifdef SQUID_DEBUG_MYSQL
	std::cout << "preparing: " << query << "\n";
#endif

	std::shared_ptr<MYSQL_STMT> stmt{ mysql_stmt_init(&connection), mysql_stmt_close };

	if (!stmt)
	{
		throw error{ "mysql_stmt_init failed", connection };
	}

	if (0 != mysql_stmt_prepare(stmt.get(), query.c_str(), static_cast<unsigned long>(query.length())))
	{
		throw error{ "mysql_stmt_prepare failed", connection };
	}

	return stmt;
}

} // namespace

class statement::impl
{
	std::shared_ptr<MYSQL>            connection_;
	std::unique_ptr<mysql_query>      query_;
	bool                              reuse_statement_;
	std::unique_ptr<query_parameters> parameters_;
	std::unique_ptr<query_results>    query_results_;
	std::shared_ptr<MYSQL_STMT>       statement_;

public:
	impl(std::shared_ptr<MYSQL> connection, std::string_view query, bool reuse_statement)
	    : connection_{ connection }
	    , query_{ std::make_unique<mysql_query>(query) }
	    , reuse_statement_{ reuse_statement }
	    , parameters_{}
	    , query_results_{}
	    , statement_{}
	{
		assert(this->connection_);
	}

	template<typename ResultsContainer>
	void execute(const std::map<std::string, parameter>& parameters, const ResultsContainer& results)
	{
		assert(this->connection_);

		this->query_results_.reset();

		if (this->statement_ && !this->reuse_statement_)
		{
			this->statement_.reset();
		}

		if (!this->statement_)
		{
			this->statement_ = prepare_statement(*this->connection_, this->query_->query());
		}

		this->parameters_ = std::make_unique<query_parameters>(*this->query_, parameters);

		this->parameters_->bind(*this->statement_);

		if (0 != mysql_stmt_execute(this->statement_.get()))
		{
			throw error{ "mysql_stmt_execute failed", *this->statement_ };
		}

		this->query_results_ = std::make_unique<query_results>(this->statement_, results);

		// This call fetches the complete result on the client side, which can be suboptimal.
		// This should not be necessary, but without this it is not possible
		// to execute this statement again while another statement exists that has a
		// result set that has not yet been fully fetched.
		// This is a limitation of the MySQL client library.
		// An application using the MySQL API directly could work around this but, since
		// this is a library, the order of operations is unpredictable.
		if (0 != mysql_stmt_store_result(this->statement_.get()))
		{
			throw error{ "mysql_stmt_store_result failed", *this->statement_ };
		}
	}

	bool fetch()
	{
		if (this->query_results_)
		{
			return this->query_results_->fetch();
		}
		else
		{
			throw error{ "Cannot fetch row from a statement that has not been executed" };
		}
	}

	std::size_t field_count()
	{
		if (this->query_results_)
		{
			return this->query_results_->field_count();
		}
		else
		{
			throw error{ "Cannot get field count from a statement that has not been executed" };
		}
	}

	std::string field_name(std::size_t index)
	{
		if (this->query_results_)
		{
			return std::string{ this->query_results_->field_name(index) };
		}
		else
		{
			throw error{ "Cannot get field name from a statement that has not been executed" };
		}
	}

	std::uint64_t affected_rows()
	{
		return mysql_affected_rows(this->connection_.get());
	}

	MYSQL_STMT& handle() const
	{
		if (!this->statement_)
		{
			throw error{ "Statement has not been executed" };
		}
		else
		{
			return *this->statement_;
		}
	}
};

statement::statement(std::shared_ptr<MYSQL> connection, std::string_view query, bool reuse_statement)
    : ibackend_statement{}
    , pimpl_{ std::make_unique<impl>(connection, query, reuse_statement) }
{
}

statement::~statement() noexcept
{
}

void statement::execute(const std::map<std::string, parameter>& parameters, const std::vector<result>& results)
{
	this->pimpl_->execute(parameters, results);
}

void statement::execute(const std::map<std::string, parameter>& parameters, const std::map<std::string, result>& results)
{
	this->pimpl_->execute(parameters, results);
}

bool statement::fetch()
{
	return this->pimpl_->fetch();
}

std::size_t statement::field_count()
{
	return this->pimpl_->field_count();
}

std::string statement::field_name(std::size_t index)
{
	return this->pimpl_->field_name(index);
}

std::uint64_t statement::affected_rows()
{
	return this->pimpl_->affected_rows();
}

/*static*/ void statement::execute(MYSQL& connection, const std::string& query)
{
	if (0 != mysql_real_query(&connection, query.c_str(), query.length()))
	{
		throw error{ "mysql_real_query failed", connection };
	}
}

MYSQL_STMT& statement::handle() const
{
	return this->pimpl_->handle();
}

} // namespace mysql
} // namespace squid
