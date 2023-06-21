//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/sqlite3/statement.h"
#include "squid/sqlite3/error.h"

#include "squid/sqlite3/detail/queryparameters.h"
#include "squid/sqlite3/detail/queryresults.h"

#include <sstream>
#include <cassert>

#ifdef SQUID_DEBUG_SQLITE
#include <iostream>
#endif

#include <sqlite3.h>

namespace squid {
namespace sqlite {

namespace {

sqlite3_stmt* prepare_statement(sqlite3& connection, const std::string& query)
{
#ifdef SQUID_DEBUG_SQLITE
	std::cout << "preparing: " << query << "\n";
#endif

	sqlite3_stmt* stmt{ nullptr };
	auto          rc = sqlite3_prepare_v2(&connection, query.c_str(), -1, &stmt, nullptr);

	if (SQLITE_OK != rc)
	{
		throw error{ "sqlite3_prepare_v2 failed", connection };
	}
	else if (!stmt)
	{
		throw error{ "sqlite3_prepare_v2 did not set the statement handle", connection };
	}
	else
	{
		return stmt;
	}
}

} // namespace

class statement::impl
{
	std::shared_ptr<sqlite3>       connection_;
	std::string                    query_;
	bool                           reuse_statement_;
	std::shared_ptr<sqlite3_stmt>  statement_;
	int                            step_result_;
	std::unique_ptr<query_results> query_results_;

	void step()
	{
		assert(this->statement_ && this->connection_);

		this->step_result_ = sqlite3_step(this->statement_.get());

		if (SQLITE_DONE != this->step_result_ && SQLITE_ROW != this->step_result_)
		{
			this->statement_.reset();
			throw error{ "sqlite3_step failed", *this->connection_ };
		}
	}

public:
	impl(std::shared_ptr<sqlite3> connection, std::string_view query, bool reuse_statement)
	    : connection_{ connection }
	    , query_{ query }
	    , reuse_statement_{ reuse_statement }
	    , statement_{}
	    , step_result_{ -1 }
	    , query_results_{}
	{
		assert(this->connection_);
	}

	template<typename ResultsContainer>
	void execute(const std::map<std::string, parameter>& parameters, const ResultsContainer& results)
	{
		assert(this->connection_);

		this->query_results_.reset();

		if (this->statement_)
		{
			if (this->reuse_statement_)
			{
				if (SQLITE_OK != sqlite3_reset(this->statement_.get()))
				{
					throw error{ "sqlite3_reset failed", *this->connection_ };
				}
			}
			else
			{
				this->statement_.reset();
			}
		}

		if (!this->statement_)
		{
			this->statement_.reset(prepare_statement(*this->connection_, this->query_), sqlite3_finalize);
		}

		query_parameters::bind(*this->connection_, *this->statement_, parameters);

		this->step();

		this->query_results_ = std::make_unique<query_results>(this->connection_, this->statement_, results);
	}

	bool fetch()
	{
		if (!this->statement_ || !this->query_results_)
		{
			throw error{ "Cannot fetch row from a statement that has not been executed" };
		}

		if (SQLITE_DONE == this->step_result_)
		{
			return false;
		}
		assert(SQLITE_ROW == this->step_result_);

		this->query_results_->fetch();

		this->step();

		return true;
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
			return this->query_results_->field_name(index);
		}
		else
		{
			throw error{ "Cannot get field name from a statement that has not been executed" };
		}
	}

	std::uint64_t affected_rows()
	{
		if (this->statement_)
		{
			return static_cast<std::uint64_t>(sqlite3_changes64(this->connection_.get()));
		}
		else
		{
			throw error{ "Cannot get the number of affected rows from a statement that has not been executed" };
		}
	}
};

statement::~statement() noexcept
{
}

statement::statement(std::shared_ptr<sqlite3> connection, std::string_view query, bool reuse_statement)
    : ibackend_statement{}
    , pimpl_{ std::make_unique<impl>(connection, query, reuse_statement) }
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

void statement::execute(sqlite3& connection, const std::string& query)
{
	std::shared_ptr<sqlite3_stmt> statement{ prepare_statement(connection, query), sqlite3_finalize };

	auto rc = sqlite3_step(statement.get());
	if (SQLITE_DONE != rc && SQLITE_ROW != rc)
	{
		throw error{ "sqlite3_step failed", connection };
	}
}

} // namespace sqlite
} // namespace squid
