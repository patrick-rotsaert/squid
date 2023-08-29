//
// Copyright (C) 2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "squid/api.h"
#include "squid/config.h"
#include "squid/postgresql/connection.h"
#include "squid/postgresql/detail/libpqfwd.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>

#include <memory>
#include <string>
#include <functional>

namespace squid {
namespace postgresql {

class SQUID_EXPORT notify_listener : public std::enable_shared_from_this<notify_listener>
{
	using callback_type     = std::function<void(const std::string& channel, int pid)>;
	using stream_descriptor = boost::asio::posix::stream_descriptor;

	stream_descriptor stream_;
	connection        connection_;
	callback_type     callback_;
	PGconn&           native_conn_;

	void on_wait(const boost::system::error_code& ec);
	void async_wait();

public:
	notify_listener(boost::asio::io_context& ioc, connection&& connection, const std::string& channel, callback_type&& callback);

	void run();
};

} // namespace postgresql
} // namespace squid

#ifndef SQUID_HAVE_BOOST
#include "notify_listener.ipp"
#endif
