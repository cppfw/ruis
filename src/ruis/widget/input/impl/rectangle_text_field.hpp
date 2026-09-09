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

#include "../../base/wrapped_widget.hpp"
#include "../../label/rectangle.hpp"
#include "../text_field.hpp"

namespace ruis {

/**
 * @brief Text field widget with a rectangle background.
 * This widget combines a rectangle (with stroke and fill colors from style)
 * and a bare_text_field as its child.
 */
class rectangle_text_field :
	virtual public widget, //
	private rectangle,
	public text_field
{
public:
	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
		container::parameters container_params;
		padding::parameters padding_params;
		rectangle::parameters rectangle_params;
		color_widget::parameters color_params;
		text_widget::parameters text_widget_params;
		bare_text_field::parameters text_field_params;
	};

private:
	rectangle_text_field(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters& params,
		utki::shared_ref<ruis::bare_text_field> bare_text_field
	);

public:
	rectangle_text_field(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params,
		ruis::string text
	);

	void on_focus_change() override;
};

namespace make {
utki::shared_ref<ruis::rectangle_text_field> rectangle_text_field(
	const utki::shared_ref<ruis::context>& context, //
	ruis::rectangle_text_field::all_parameters params,
	ruis::string text
);
} // namespace make

} // namespace ruis