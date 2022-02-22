//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "basicstatement.h"
#include "ibackendstatement.h"

#include <cassert>

namespace squid {

BasicStatement::BasicStatement(std::unique_ptr<IBackendStatement>&& statement)
    : parameters_{}
    , results_{}
    , statement_{ std::move(statement) }
{
}

BasicStatement::~BasicStatement() noexcept
{
}

BasicStatement& BasicStatement::bind(std::string_view name, const unsigned char* value, std::size_t size)
{
	this->upsertParameter(name, value, size);
	return *this;
}

void BasicStatement::execute()
{
	assert(this->statement_);
	this->statement_->execute(this->parameters_);
}

bool BasicStatement::fetch()
{
	assert(this->statement_);
	return this->statement_->fetch(this->results_);
}

} // namespace squid
