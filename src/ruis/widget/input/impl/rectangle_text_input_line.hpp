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

#include "../../base/decorated_widget.hpp"
#include "../../label/rectangle.hpp"
#include "../text_input_line.hpp"

namespace ruis {

/**
 * @brief Text input line widget with a rectangle background.
 * This widget combines a rectangle (with stroke and fill colors from style)
 * and a text_input_line as its child.
 */
class rectangle_text_input_line :
	virtual public widget, //
	private rectangle,
	public decorated_widget<text_input_line>
{
public:
	struct parameters {
		// Additional parameters can be added here if needed
	};

	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
		container::parameters container_params;
		padding::parameters padding_params;
		color_widget::parameters color_params;
		rectangle::parameters rectangle_params;
		text_input_line::all_parameters text_input_line_params;
	};

private:
	rectangle_text_input_line(
		utki::shared_ref<ruis::context>& context, //
		all_parameters& params,
		utki::shared_ref<ruis::text_input_line> text_input_line
	);

public:
	rectangle_text_input_line(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		ruis::string text
	);

	void on_focus_change() override;
};

namespace make {
utki::shared_ref<ruis::rectangle_text_input_line> rectangle_text_input_line(
	utki::shared_ref<ruis::context> context, //
	ruis::rectangle_text_input_line::all_parameters params,
	ruis::string text
);
} // namespace make

} // namespace ruis