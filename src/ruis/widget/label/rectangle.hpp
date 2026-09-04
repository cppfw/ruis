/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include "../../paint/rectangle_vao.hpp"
#include "../base/color_widget.hpp"

#include "padding.hpp"

namespace ruis {

/**
 * @brief Rectangle widget.
 * This is a widget which can display a rectangle of a single color.
 */
class rectangle :
	public padding, //
	public color_widget
{
public:
	struct parameters {
		/**
		 * @brief Corner radii of the rectangle.
		 * The order of the radii is: left-top, right-top, right-bottom, left-bottom.
		 */
		corners<styled<length>> corner_radii;

		/**
		 * @brief Stroke width of the rectangle.
		 * Undefined value means no stroke.
		 */
		styled<length> stroke_width;

		/**
		 * @brief Stroke color of the rectangle.
		 */
		// stroke_color is red by default to make it easily noticable if it is used without being set to some other color
		styled<ruis::color> stroke_color = ruis::color(0xff, 0, 0, 0xff);
	};

private:
	parameters params;

public:
	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
		container::parameters container_params;
		padding::parameters padding_params;
		color_widget::parameters color_params;
		parameters rectangle_params;
	};

	rectangle(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		widget_list children
	);

	rectangle(const rectangle&) = delete;
	rectangle& operator=(const rectangle&) = delete;

	rectangle(rectangle&&) = delete;
	rectangle& operator=(rectangle&&) = delete;

	~rectangle() override = default;

	void render(const ruis::mat4& matrix) const override;

private:
	bool has_stroke() const
	{
		return !this->params.stroke_width.get().is_undefined();
	}

	void update_vaos();

	ruis::paint::rectangle_vao fill_vao;
	ruis::paint::rectangle_vao stroke_vao;
};

namespace make {
utki::shared_ref<ruis::rectangle> rectangle(
	utki::shared_ref<ruis::context> context,
	rectangle::all_parameters params,
	widget_list children = {}
);
} // namespace make

} // namespace ruis
