//
// Copyright (C) 2022-2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/basicstatement.h"
#include "squid/ibackendstatement.h"

#include <cassert>

namespace squid {

basic_statement::basic_statement(std::unique_ptr<ibackend_statement>&& statement)
    : parameters_{}
    , results_{}
    , named_results_{}
    , statement_{ std::move(statement) }
{
}

basic_statement::~basic_statement() noexcept
{
}

basic_statement& basic_statement::bind(std::string_view name, const unsigned char* value, std::size_t size)
{
	this->upsert_parameter(name, byte_string_view{ value, size }, parameter::by_value{});
	return *this;
}

void basic_statement::execute()
{
	assert(this->statement_);
	this->statement_->execute(this->parameters_);
}

bool basic_statement::fetch()
{
	assert(this->statement_);
	if (!this->results_.empty() && !this->named_results_.empty())
	{
		throw error{ "Named result binding cannot be combined with sequential result binding" };
	}
	if (!this->named_results_.empty())
	{
		return this->statement_->fetch(this->named_results_);
	}
	else
	{
		return this->statement_->fetch(this->results_);
	}
}

std::size_t basic_statement::field_count()
{
	assert(this->statement_);
	return this->statement_->field_count();
}

std::string basic_statement::field_name(std::size_t index)
{
	assert(this->statement_);
	return this->statement_->field_name(index);
}

} // namespace squid
