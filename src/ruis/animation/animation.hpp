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

class animation :
	virtual public utki::shared, //
	private updateable
{
	utki::shared_ref<ruis::updater> updater;

	uint32_t duration_ms;
	uint32_t left_ms = 0;

	void update(uint32_t dt) override;

	std::function<void(real factor)> update_handler;

protected:
	virtual void on_update(real factor);

public:
	animation( //
		utki::shared_ref<ruis::updater> updater,
		uint32_t duration_ms,
		decltype(update_handler) update_handler
	);

	bool is_running() const noexcept
	{
		return this->is_updating();
	}

	void start();
	void stop() noexcept;
};

} // namespace ruis
