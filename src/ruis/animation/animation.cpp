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

void animation::update(uint32_t dt)
{
	uint32_t over_end_ms = 0;
	if (this->left_ms <= dt) {
		over_end_ms = dt - this->left_ms;
		this->left_ms = 0;
	} else {
		this->left_ms -= dt;
	}

	auto factor = real(this->duration_ms - this->left_ms) / real(this->duration_ms);

	this->on_update(factor);

	if (this->left_ms == 0) {
		this->stop();
		this->on_end(over_end_ms);
	}
}

void animation::start()
{
	this->updater.get().start(utki::make_weak_from(static_cast<updateable&>(*this)));
}

void animation::stop() noexcept
{
	this->updater.get().stop(*this);
}

void animation::reset()
{
	this->stop();
	this->left_ms = this->duration_ms;
}
