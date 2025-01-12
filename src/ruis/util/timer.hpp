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

#include "../updateable.hpp"
#include "../updater.hpp"

namespace ruis {

/**
 * @brief Timer.
 * The class is not thread safe and is supposed to be only used from
 * within the UI-thread. Timer callback is invoked from UI-thread.
 */
class timer :
	virtual public utki::shared, //
	private updateable
{
	utki::shared_ref<ruis::updater> updater;

public:
	/**
	 * @brief Timer expired callback type.
	 * @param elapsed_ms - actual number of milliseconds elapsed since timer start.
	 */
	using expire_handler_type = std::function<void(uint32_t elapsed_ms)>;

private:
	uint32_t timeout_ms = 0;
	uint32_t left_ms = 0;

	expire_handler_type expire_handler;

	void update(uint32_t dt_ms) override;

public:
	/**
	 * @brief Constructor.
	 * The timer is supposed to be dynamically allocated.
	 * @param updater - updater to use for running the timer.
	 * @param expire_handler - callback to invoke when timer expires.
	 */
	timer( //
		utki::shared_ref<ruis::updater> updater,
		expire_handler_type expire_handler
	);

	/**
	 * @brief Get current timeout.
	 * Get the timer's timeout which was used to start the timer.
	 * @return Current timeout of the timer in milliseconds.
	 */
	uint32_t get_timeout_ms() const noexcept
	{
		return this->timeout_ms;
	}

	/**
	 * @brief Check if the timer is running.
	 * @return true if the timer is currently running, i.e. it was started and has not expired.
	 * @return false otherwise.
	 */
	bool is_running() const noexcept
	{
		return this->is_updating();
	}

	/**
	 * @brief Start the timer.
	 * @param timeout_ms - timeout in milliseconds to set the timer to expire in.
	 */
	void start(uint32_t timeout_ms);

	/**
	 * @brief Stop the timer.
	 */
	void stop() noexcept;
};

} // namespace ruis
