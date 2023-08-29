//
// Copyright (C) 2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "squid/postgresql/notify_listener.hpp"
#include "squid/postgresql/backendconnection.h"
#include "squid/postgresql/error.h"

#include <boost/asio/post.hpp>

#include <libpq-fe.h>

namespace squid {
namespace postgresql {

void notify_listener::on_wait(const boost::system::error_code& ec)
{
	if (!ec)
	{
		auto conn = &this->native_conn_;
		PQconsumeInput(conn);
		while (auto notify = PQnotifies(conn))
		{
			auto channel = std::string{ notify->relname };
			auto pid     = notify->be_pid;
			PQfreemem(notify);
			boost::asio::post(this->stream_.get_executor(),
			                  [channel = std::move(channel), pid, self = this->shared_from_this()]() { self->callback_(channel, pid); });
			PQconsumeInput(conn);
		}
		this->async_wait();
	}
}

void notify_listener::async_wait()
{
	this->stream_.async_wait(stream_descriptor::wait_read,
	                         std::bind(&notify_listener::on_wait, this->shared_from_this(), std::placeholders::_1));
}

notify_listener::notify_listener(boost::asio::io_context& ioc,
                                 connection&&             connection,
                                 const std::string&       channel,
                                 callback_type&&          callback)
    : stream_{ ioc }
    , connection_{ std::move(connection) }
    , callback_{ std::move(callback) }
    , native_conn_{ connection_.backend().handle() }
{
	this->connection_.execute("LISTEN " + channel);

	auto sock = PQsocket(&this->native_conn_);
	if (sock < 0)
	{
		throw error{ "PQsocket failed", this->native_conn_ };
	}

	this->stream_.assign(sock);
}

void notify_listener::run()
{
	this->async_wait();
}

} // namespace postgresql
} // namespace squid
