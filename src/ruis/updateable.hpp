/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#pragma once

#include <cstdint>

#include <utki/shared.hpp>

#include "updater.hpp"

namespace ruis {

/**
 * @brief Base class for periodic updating.
 * A subclass of this class can subscribe for periodic updates to be performed from UI thread.
 * For example every 30 milliseconds it will call the update() method.
 */
class updateable : virtual public utki::shared
{
	friend class gui;
	friend class updater;

private:
	uint16_t dt = 0;

	uint32_t started_at = 0;

	uint32_t ends_at() const noexcept
	{
		return this->started_at + uint32_t(this->dt);
	}

	bool updating = false;

	// pointer to the queue the updateable is inserted into
	updater::update_queue* queue = nullptr;

	updater::update_queue::iterator iter; // iterator into the queue.

	bool pending_addition = false;

public:
	/**
	 * @brief Check if the object is currently subscribed for updates.
	 * @return true if object is subscribed for updates.
	 * @return false otherwise.
	 */
	bool is_updating() const noexcept
	{
		return this->updating;
	}

	/**
	 * @brief A method to perform an update.
	 * Override this method to perform an update.
	 * @param dt_ms - actual time elapsed since the previous update.
	 */
	virtual void update(uint32_t dt_ms) = 0;

	updateable() = default;

	updateable(const updateable&) = delete;
	updateable& operator=(const updateable&) = delete;

	updateable(updateable&&) = delete;
	updateable& operator=(updateable&&) = delete;

	~updateable() override = default;
};

} // namespace ruis
