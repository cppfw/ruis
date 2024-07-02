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

#include "../updateable.hpp"
#include "../updater.hpp"

namespace ruis {

class timer : private updateable
{
	utki::shared_ref<ruis::updater> updater;

public:
	using expire_handler_type = std::function<void(uint32_t elapsed_ms)>;

private:
	uint32_t timeout_ms = 0;
	uint32_t left_ms = 0;

	expire_handler_type expire_handler;

	void update(uint32_t dt_ms) override;

public:
	timer( //
		utki::shared_ref<ruis::updater> updater,
		expire_handler_type on_expire
	);

	uint32_t get_timeout_ms() const noexcept
	{
		return this->timeout_ms;
	}

	bool is_running() const noexcept
	{
		return this->is_updating();
	}

	void start(uint32_t timeout_ms);

	void stop() noexcept;
};

} // namespace ruis
