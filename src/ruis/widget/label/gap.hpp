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

#include "../base/color_widget.hpp"

namespace ruis {

class gap : public color_widget
{
public:
	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
		color_widget::parameters color_params = {
			.color = {},
			.disabled_color = {} //
		};
	};

	gap( //
		utki::shared_ref<ruis::context> context,
		all_parameters params //
	);

	void render(const mat4& matrix) const override;
};

namespace make {
inline utki::shared_ref<ruis::gap> gap(
	utki::shared_ref<ruis::context> context, //
	ruis::gap::all_parameters params
)
{
	return utki::make_shared<ruis::gap>( //
		std::move(context),
		std::move(params)
	);
}
} // namespace make

} // namespace ruis
