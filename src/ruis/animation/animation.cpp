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

#include "animation.hpp"

using namespace ruis;

animation::animation( //
	utki::shared_ref<ruis::updater> updater,
	uint32_t duration_ms
) :
	updater(std::move(updater)),
	duration_ms(duration_ms),
	left_ms(duration_ms)
{}

void animation::on_update(real factor)
{
	if (this->update_handler) {
		this->update_handler(factor);
	}
}

void animation::on_end(uint32_t over_end_ms)
{
	if (this->end_handler) {
		this->end_handler(over_end_ms);
	}
}

uint32_t animation::forward(uint32_t dt)
{
	if (this->left_ms <= dt) {
		uint32_t over_end_ms = dt - this->left_ms;
		this->left_ms = 0;
		return over_end_ms;
	}

	this->left_ms -= dt;
	return 0;
}

void animation::update(uint32_t dt)
{
	auto over_end_ms = this->forward(dt);

	if (this->is_ended()) {
		this->stop();
		this->on_update(real(1));
		this->on_end(over_end_ms);
	} else {
		this->on_update(this->get_factor());
	}
}

void animation::start(uint32_t dt_ms)
{
	// This will add the updatable to an active list, but will not call the update() immediately.
	this->updater.get().start( //
		utki::make_shared_from( //
			static_cast<updateable&>(*this)
		)
	);

	// This call to update() can potentially stop the started updating,
	// this is why it is called after the updating has been started.
	if (dt_ms != 0) {
		this->update(dt_ms);
	}
}

void animation::stop() noexcept
{
	this->updater.get().stop(*this);
}
