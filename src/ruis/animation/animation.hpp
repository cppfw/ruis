/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

// Lint complains that std::shared_from_this is privately inherited via ruis::updateable
// and trying to use it will result in undefined behaviour. This is false, because
// ruis::updateable virtually iherits from utki::shared which inherits
// from std::shared_from_this, but ruis::animation also publicly and virtually inherits utki::shared,
// so std::shared_from_this inheritance is effectively public.
// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this)
class animation :
	virtual public utki::shared, //
	private updateable
{
	utki::shared_ref<ruis::updater> updater;

	uint32_t duration_ms;
	uint32_t left_ms = 0;

	void update(uint32_t dt) override;

protected:
	virtual void on_update(real factor);
	virtual void on_end(uint32_t over_end_ms);

	real get_factor() const
	{
		return real(this->duration_ms - this->left_ms) / real(this->duration_ms);
	}

	bool is_ended() const
	{
		return this->left_ms == 0;
	}

	/**
	 * @brief Move animation forward.
	 * @return Number of milliseconds over the end if the animation has ended.
	 */
	uint32_t forward(uint32_t dt);

public:
	std::function<void(real factor)> update_handler;
	std::function<void(uint32_t over_end_ms)> end_handler;

	animation( //
		utki::shared_ref<ruis::updater> updater,
		uint32_t duration_ms
	);

	void start();
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
