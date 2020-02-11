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
	uint16_t dt;

	uint32_t startedAt; // timestamp when update timer started.

	uint32_t endAt()const noexcept{
		return this->startedAt + uint32_t(this->dt);
	}

	bool is_updating_v = false;

	// pointer to the queue the updateable is inserted into
	updater::UpdateQueue* queue = nullptr;

	updater::UpdateQueue::iterator iter; // iterator into the queue.

	bool pendingAddition = false;

public:
	/**
	 * @brief Check if the object is currently subscribed for updates.
	 * @return true if object is subscribed for updates.
	 * @return false otherwise.
	 */
	bool is_updating()const noexcept{
		return this->is_updating_v;
	}

	/**
	 * @brief A method to perform an update.
	 * Override this method to perform an update.
	 * @param dt_ms - actual time elapsed since the previous update.
	 */
	virtual void update(uint32_t dt_ms) = 0;
};

}
