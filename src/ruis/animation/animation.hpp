/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include "../config.hpp"
#include "../updateable.hpp"
#include "../updater.hpp"

namespace ruis {

/**
 * @brief A time-based animation.
 * An animation runs for a fixed duration and calls an update handler with a
 * progress factor between 0 and 1 as it advances, and an end handler when it
 * finishes.
 */

// Lint complains that std::shared_from_this is privately inherited via ruis::updateable
// and trying to use it will result in undefined behaviour. This is false, because
// ruis::updateable virtually iherits from utki::shared which inherits
// from std::shared_from_this, but ruis::animation also publicly and virtually inherits utki::shared,
// so std::shared_from_this inheritance is effectively public.
// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::shared_from_this is public via utki::shared")
class animation :
	virtual public utki::shared, //
	private updateable
{
	utki::shared_ref<ruis::updater> updater;

	uint32_t duration_ms;
	uint32_t left_ms = 0;

	/**
	 * @brief Advance the animation.
	 * @param dt - number of milliseconds to advance the animation.
	 */
	void update(uint32_t dt) override;

protected:
	/**
	 * @brief Called on each update of the animation.
	 * @param factor - animation progress factor in [0:1] range.
	 */
	virtual void on_update(real factor);

	/**
	 * @brief Called when the animation has ended.
	 * @param over_end_ms - number of milliseconds the animation was forwarded over its end.
	 */
	virtual void on_end(uint32_t over_end_ms);

	/**
	 * @brief Get animation progress factor.
	 * @return animation progress factor in [0:1] range.
	 */
	real get_factor() const
	{
		return real(this->duration_ms - this->left_ms) / real(this->duration_ms);
	}

	/**
	 * @brief Check if the animation has ended.
	 * @return true if the animation has ended.
	 */
	bool is_ended() const
	{
		return this->left_ms == 0;
	}

	/**
	 * @brief Move animation forward.
	 * @param dt_ms - number of milliseconds to forward the animation.
	 * @return Number of milliseconds over the end if the animation has ended.
	 */
	uint32_t forward(uint32_t dt_ms);

public:
	/**
	 * @brief Animation update handler.
	 * Invoked on each update of the animation with the progress factor in [0:1] range.
	 */
	std::function<void(real factor)> update_handler;

	/**
	 * @brief Animation end handler.
	 * Invoked when the animation has ended with the number of milliseconds
	 * the animation was forwarded over its end.
	 */
	std::function<void(uint32_t over_end_ms)> end_handler;

	/**
	 * @brief Constructor.
	 * @param updater - updater which will drive the animation.
	 * @param duration_ms - duration of the animation in milliseconds.
	 */
	animation( //
		utki::shared_ref<ruis::updater> updater,
		uint32_t duration_ms
	);

	/**
	 * @brief Play the animation.
	 * Starts the animation from the beginning.
	 * @param dt_ms - number of milliseconds to immediately forward the animation by.
	 */
	void play(uint32_t dt_ms = 0);

	/**
	 * @brief Stop the animation.
	 */
	void stop() noexcept;

	/**
	 * @brief Rewind the animation.
	 * Rewind to beginning.
	 */
	void rewind()
	{
		this->left_ms = this->duration_ms;
	}
};

} // namespace ruis
