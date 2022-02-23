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

class BackendConnectionWrapper : public IBackendConnection
{
	using ReleaseFunction = std::function<void(std::shared_ptr<IBackendConnection>&&)>;

	std::shared_ptr<IBackendConnection> connection_;
	ReleaseFunction                     releaseFunction_;

	std::unique_ptr<IBackendStatement> createStatement(std::string_view query) override
	{
		return this->connection_->createStatement(query);
	}

	std::unique_ptr<IBackendStatement> createPreparedStatement(std::string_view query) override
	{
		return this->connection_->createPreparedStatement(query);
	}

public:
	explicit BackendConnectionWrapper(std::shared_ptr<IBackendConnection>&& connection, ReleaseFunction&& releaseFunction)
	    : connection_{ std::move(connection) }
	    , releaseFunction_{ std::move(releaseFunction) }
	{
	}

	~BackendConnectionWrapper()
	{
		this->releaseFunction_(std::move(this->connection_));
	}
};

} // namespace

class ConnectionPool::impl
{
	std::queue<std::shared_ptr<IBackendConnection>> queue_;
	std::mutex                                      mutex_;
	std::condition_variable                         cv_;

	using lock_type = std::unique_lock<std::mutex>;

public:
	impl(const IBackendConnectionFactory& backendConnectionFactory, std::string_view connectionInfo, std::size_t count)
	{
		if (count == 0)
		{
			throw std::invalid_argument{ "count must be greater than zero" };
		}

		while (count--)
		{
			this->queue_.push(backendConnectionFactory.createBackendConnection(connectionInfo));
		}
	}

	/// Waits indefinitely until the pool has a connection available.
	std::shared_ptr<IBackendConnection> acquire()
	{
		lock_type lock(mutex_);

		while (this->queue_.empty())
		{
			this->cv_.wait(lock);
		}

		return this->acquireFromFrontOfQueue(lock);
	}

	/// Returns nullptr if no connection is available within the specified timeout.
	std::shared_ptr<IBackendConnection> acquire(const std::chrono::milliseconds& timeout)
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

	std::shared_ptr<IBackendConnection> tryAcquire()
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
	std::shared_ptr<IBackendConnection> acquireFromFrontOfQueue(lock_type&)
	{
		auto connection = this->queue_.front();
		this->queue_.pop();

		return std::make_shared<BackendConnectionWrapper>(
		    std::move(connection), [this](std::shared_ptr<IBackendConnection>&& connection) { this->release(std::move(connection)); });
	}

	void release(std::shared_ptr<IBackendConnection>&& connection)
	{
		{
			lock_type lock(mutex_);
			this->queue_.push(connection);
		}
		this->cv_.notify_one();
	}
};

ConnectionPool::ConnectionPool(const IBackendConnectionFactory& backendConnectionFactory,
                               std::string_view                 connectionInfo,
                               std::size_t                      count)
    : pimpl_{ std::make_unique<impl>(backendConnectionFactory, connectionInfo, count) }
{
}

std::shared_ptr<IBackendConnection> ConnectionPool::acquire()
{
	return this->pimpl_->acquire();
}

std::shared_ptr<IBackendConnection> ConnectionPool::acquire(const std::chrono::milliseconds& timeout)
{
	return this->pimpl_->acquire(timeout);
}

std::shared_ptr<IBackendConnection> ConnectionPool::tryAcquire()
{
	return this->pimpl_->tryAcquire();
}

} // namespace squid
