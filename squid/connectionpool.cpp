#include "squid/connectionpool.h"
#include "squid/ibackendconnection.h"
#include "squid/ibackendconnectionfactory.h"
#include "squid/ibackendstatement.h"
#include "squid/error.h"

#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace squid {

namespace {

class BackendConnectionWrapper : public ibackend_connection
{
	using ReleaseFunction = std::function<void(std::shared_ptr<ibackend_connection>&&)>;

	std::shared_ptr<ibackend_connection> connection_;
	ReleaseFunction                      release_function_;

	std::unique_ptr<ibackend_statement> create_statement(std::string_view query) override
	{
		return this->connection_->create_statement(query);
	}

	std::unique_ptr<ibackend_statement> create_prepared_statement(std::string_view query) override
	{
		return this->connection_->create_prepared_statement(query);
	}

	void execute(const std::string& query) override
	{
		this->connection_->execute(query);
	}

public:
	explicit BackendConnectionWrapper(std::shared_ptr<ibackend_connection>&& connection, ReleaseFunction&& releaseFunction)
	    : connection_{ std::move(connection) }
	    , release_function_{ std::move(releaseFunction) }
	{
	}

	~BackendConnectionWrapper()
	{
		this->release_function_(std::move(this->connection_));
	}
};

} // namespace

class connection_pool::impl
{
	std::queue<std::shared_ptr<ibackend_connection>> queue_;
	std::mutex                                       mutex_;
	std::condition_variable                          cv_;

	using lock_type = std::unique_lock<std::mutex>;

public:
	impl(const ibackend_connection_factory& factory, std::string_view connection_info, std::size_t count)
	{
		if (count == 0)
		{
			throw std::invalid_argument{ "count must be greater than zero" };
		}

		while (count--)
		{
			this->queue_.push(factory.create_backend_connection(connection_info));
		}
	}

	/// Waits indefinitely until the pool has a connection available.
	std::shared_ptr<ibackend_connection> acquire()
	{
		lock_type lock(mutex_);

		while (this->queue_.empty())
		{
			this->cv_.wait(lock);
		}

		return this->acquireFromFrontOfQueue(lock);
	}

	/// Returns nullptr if no connection is available within the specified timeout.
	std::shared_ptr<ibackend_connection> acquire(const std::chrono::milliseconds& timeout)
	{
		lock_type lock(mutex_);

		while (this->queue_.empty())
		{
			if (this->cv_.wait_for(lock, timeout) == std::cv_status::timeout)
			{
				return nullptr;
			}
		}

		return this->acquireFromFrontOfQueue(lock);
	}

	std::shared_ptr<ibackend_connection> tryAcquire()
	{
		lock_type lock(mutex_);

		if (this->queue_.empty())
		{
			return nullptr;
		}
		else
		{
			return this->acquireFromFrontOfQueue(lock);
		}
	}

private:
	std::shared_ptr<ibackend_connection> acquireFromFrontOfQueue(lock_type&)
	{
		auto connection = this->queue_.front();
		this->queue_.pop();

		return std::make_shared<BackendConnectionWrapper>(
		    std::move(connection), [this](std::shared_ptr<ibackend_connection>&& connection) { this->release(std::move(connection)); });
	}

	void release(std::shared_ptr<ibackend_connection>&& connection)
	{
		{
			lock_type lock(mutex_);
			this->queue_.push(connection);
		}
		this->cv_.notify_one();
	}
};

connection_pool::connection_pool(const ibackend_connection_factory& factory,
                                 std::string_view                   connection_info,
                                 std::size_t                        count)
    : pimpl_{ std::make_unique<impl>(factory, connection_info, count) }
{
}

std::shared_ptr<ibackend_connection> connection_pool::acquire()
{
	return this->pimpl_->acquire();
}

std::shared_ptr<ibackend_connection> connection_pool::acquire(const std::chrono::milliseconds& timeout)
{
	return this->pimpl_->acquire(timeout);
}

std::shared_ptr<ibackend_connection> connection_pool::try_acquire()
{
	return this->pimpl_->tryAcquire();
}

} // namespace squid
