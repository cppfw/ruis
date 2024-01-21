/*
ruis - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#include "../../widget.hpp"

namespace ruis {

class ratio_proxy : virtual public widget
{
	real x_above_y = 1;

public:
	ratio_proxy(const utki::shared_ref<ruis::context>& c, const treeml::forest& desc);

	ratio_proxy(const ratio_proxy&) = delete;
	ratio_proxy& operator=(const ratio_proxy&) = delete;

	ratio_proxy(ratio_proxy&&) = delete;
	ratio_proxy& operator=(ratio_proxy&&) = delete;

	~ratio_proxy() override = default;

	void set_aspect_ratio(real x_above_y);

	real get_aspect_ratio() const noexcept
	{
		return this->x_above_y;
	}

	ruis::vector2 measure(const ruis::vector2& quotum) const override;
};

} // namespace ruis
