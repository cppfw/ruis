#pragma once

#include <cstdint>

#include <utki/shared.hpp>

#include "exception.hpp"

#include "updater.hpp"

namespace morda{

/**
 * @brief Base class for periodic updating.
 * A subclass of this class can subscribe for periodic updates to be performed from UI thread.
 * For example every 30 milliseconds it will call the update() method.
 */
class updateable : virtual public utki::shared{
	friend class gui;
	friend class updater;

private:
	std::uint16_t dt;

	std::uint32_t startedAt; // timestamp when update timer started.

	std::uint32_t endAt()const noexcept{
		return this->startedAt + std::uint32_t(this->dt);
	}

	bool isUpdating_v = false;

	// pointer to the queue the updateable is inserted into
	updater::UpdateQueue* queue = nullptr;

	updater::UpdateQueue::iterator iter; // iterator into the queue.

	bool pendingAddition = false;

public:
	/**
	 * @brief Basic update-related Exception.
	 */
	class Exc : public morda::Exc{
	public:
		Exc(const std::string& message) :
				morda::Exc(message)
		{}
	};

	/**
	 * @brief Check if the object is currently subscribed for updates.
	 * @return true if object is subscribed for updates.
	 * @return false otherwise.
	 */
	bool isUpdating()const noexcept{
		return this->isUpdating_v;
	}

	/**
	 * @brief Subscribe for updates.
	 * Normally, updates will start from the next UI cycle.
	 * The virtual update() method will be called periodically.
	 * Due to limitations specific to each particular platform the actual period
	 * can be different from requested period.
	 * The function is not thread safe.
	 * @param dtMs - time period between updates, in milliseconds.
	 */
	void startUpdating(std::uint16_t dtMs = 30);

	/**
	 * @brief Unsubscribe from updating.
	 * After calling this method there will be no updates done.
	 * The function is not thread safe.
	 */
	void stopUpdating()noexcept;

	/**
	 * @brief A method to perform an update.
	 * Override this method to perform an update.
	 * @param dtMs - actual time elapsed since the previous update.
	 */
	virtual void update(std::uint32_t dtMs) = 0;
};

}
