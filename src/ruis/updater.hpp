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

#include <functional>
#include <list>
#include <memory>

#include <utki/debug.hpp>
#include <utki/shared_ref.hpp>

namespace ruis {

class updateable;

class updater : public std::enable_shared_from_this<updater>
{
	friend class ruis::updateable;

	std::function<uint32_t()> get_ticks_ms;

	struct update_queue_item {
		uint32_t ends_at = 0;
		std::weak_ptr<ruis::updateable> updateable;
	};

	class update_queue : public std::list<update_queue_item>
	{
	public:
		update_queue::iterator insert(const update_queue_item& p);

		std::shared_ptr<ruis::updateable> pop_front()
		{
			auto ret = this->front().updateable.lock();
			this->list::pop_front();
			return ret;
		}
	};

	update_queue q1, q2;

	update_queue* active_queue;
	update_queue* inactive_queue;

	uint32_t last_updated_timestamp = 0;

	std::list<std::shared_ptr<ruis::updateable>> to_add;

	void add_pending();

	void update_updateable(const std::shared_ptr<updateable>& u);

	void remove_from_to_add(updateable* u);

	static decltype(get_ticks_ms) get_default_get_ticks_ms();

public:
	updater(std::function<uint32_t()> get_ticks_ms = get_default_get_ticks_ms()) :
		get_ticks_ms(std::move(get_ticks_ms)),
		active_queue(&q1),
		inactive_queue(&q2){ASSERT(this->get_ticks_ms)}

		// returns dt to wait before next update
		uint32_t update();

	constexpr static auto default_update_interval_ms = 16;

	/**
	 * @brief Subscribe updateable for updates.
	 * Normally, updates will start from the next UI cycle.
	 * The virtual update() method will be called periodically.
	 * Due to limitations specific to each particular platform the actual period
	 * can be different from requested period.
	 * The function is not thread safe, must be called from UI thread.
	 * @param u - updateable to start updating.
	 * @param dt_ms - time period between updates, in milliseconds.
	 */
	void start(
		const utki::shared_ref<updateable>& u, //
		uint16_t dt_ms = default_update_interval_ms
	);

	/**
	 * @brief Unsubscribe from updating.
	 * After calling this method there will be no updates done.
	 * The function is not thread safe, must be called fom UI thread.
	 * @param u - updateble to stop updating.
	 */
	void stop(updateable& u) noexcept;
};

} // namespace ruis

// include definitions for forward declared classes
#include "updateable.hpp"
