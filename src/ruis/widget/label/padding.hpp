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

#include "../base/frame_widget.hpp"

namespace ruis {

class padding : public frame_widget
{
protected:
	sides<real> get_min_borders() const noexcept override;

public:
	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		container::parameters container_params;
		frame_widget::parameters padding_params;
	};

	padding(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		widget_list children
	);

	vec2 measure(const vec2& quotum) const override;
	void on_lay_out() override;

private:
};

namespace make {
/**
 * @brief Construct 'padding' widget.
 * Default layout is pile.
 * @param context - ruis context.
 * @param params - 'padding' widget parameters.
 * @param children - children of the constructed 'padding' widget.
 * @return newly constructed 'padding' widget.
 */
utki::shared_ref<ruis::padding> padding(
	utki::shared_ref<context> context, //
	padding::all_parameters params,
	widget_list children
);
} // namespace make

} // namespace ruis
