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

#include "../widget.hpp"

namespace ruis {

class aspect_ratio_proxy : virtual public widget
{
public:
	struct parameters {
		real x_above_y = 1;
	};

private:
	parameters params;

public:
	aspect_ratio_proxy(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);

	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		parameters aspect_ratio_params;
	};

	aspect_ratio_proxy(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	);

	aspect_ratio_proxy(const aspect_ratio_proxy&) = delete;
	aspect_ratio_proxy& operator=(const aspect_ratio_proxy&) = delete;

	aspect_ratio_proxy(aspect_ratio_proxy&&) = delete;
	aspect_ratio_proxy& operator=(aspect_ratio_proxy&&) = delete;

	~aspect_ratio_proxy() override = default;

	void set_aspect_ratio(real x_above_y);

	real get_aspect_ratio() const noexcept
	{
		return this->params.x_above_y;
	}

	ruis::vector2 measure(const ruis::vector2& quotum) const override;
};

namespace make {
inline utki::shared_ref<ruis::aspect_ratio_proxy> aspect_ratio_proxy(
	utki::shared_ref<ruis::context> context, //
	ruis::aspect_ratio_proxy::all_parameters params
)
{
	return utki::make_shared<ruis::aspect_ratio_proxy>(
		std::move(context), //
		std::move(params)
	);
}
} // namespace make

} // namespace ruis
