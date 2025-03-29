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

#include "timer.hpp"

#include <limits>

#include <utki/exception.hpp>
#include <utki/string.hpp>

using namespace ruis;

timer::timer( //
	utki::shared_ref<ruis::updater> updater,
	decltype(expire_handler) expire_handler
) :
	updater(std::move(updater)),
	expire_handler(std::move(expire_handler))
{}

void timer::update(uint32_t dt_ms)
{
	if (dt_ms < this->left_ms) {
		constexpr auto max_dt_ms = std::numeric_limits<uint16_t>::max();

		this->left_ms -= dt_ms;

		if (this->left_ms > max_dt_ms) {
			if (this->left_ms < max_dt_ms) {
				this->updater.get().stop(*this);
				this->updater.get().start( //
					utki::make_shared_from(static_cast<updateable&>(*this)),
					this->left_ms
				);
			}
		}

		return;
	}

	ASSERT(dt_ms >= this->left_ms)

	this->updater.get().stop(*this);

	if (this->expire_handler) {
		auto extra_ms = dt_ms - this->left_ms;
		this->expire_handler(this->timeout_ms + extra_ms);
	}
}

void timer::start(uint32_t timeout_ms)
{
	try {
		this->updater.get().start( //
			utki::make_shared_from(static_cast<ruis::updateable&>(*this)),
			uint16_t(std::min(timeout_ms, uint32_t(std::numeric_limits<uint16_t>::max())))
		);

		this->timeout_ms = timeout_ms;
		this->left_ms = timeout_ms;
	} catch (...) {
		utki::throw_with_nested(std::runtime_error("timer::start(): failed"));
	}
}

void timer::stop() noexcept
{
	this->updater.get().stop(*this);
}
