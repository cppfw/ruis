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

#include "animation.hpp"

using namespace ruis;

animation::animation( //
	utki::shared_ref<ruis::updater> updater,
	uint32_t duration_ms,
	decltype(update_handler) update_handler
) :
	updater(std::move(updater)),
	duration_ms(duration_ms),
	update_handler(std::move(update_handler))
{}

void animation::on_update(real factor)
{
	if (this->update_handler) {
		this->update_handler(factor);
	}
}

void animation::update(uint32_t dt)
{
	if (this->left_ms <= dt) {
		this->left_ms = 0;
	} else {
		this->left_ms -= dt;
	}

	auto factor = real(this->duration_ms - this->left_ms) / real(this->duration_ms);

	this->on_update(factor);
}

void animation::start()
{
	this->updater.get().start(utki::make_weak_from(static_cast<updateable&>(*this)));
	this->left_ms = this->duration_ms;
}

void animation::stop() noexcept
{
	this->updater.get().stop(*this);
}
