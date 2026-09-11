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

#include "../labeled_text_field.hpp"

#include "rectangle_text_field.hpp"

namespace ruis {

/**
 * @brief Rectangle text field widget with a text label above it.
 * This widget combines a container laid out as a column,
 * a text label as the first child and a rectangle_text_field as the second one.
 */
class labeled_rectangle_text_field :
	private container, //
	public labeled_text_field
{
public:
	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
		labeled_widget::parameters label_params;
		rectangle_text_field::parameters rectangle_text_field_params;
	};

public:
	labeled_rectangle_text_field(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params,
		ruis::string text
	);
};

namespace make {
utki::shared_ref<ruis::labeled_rectangle_text_field> labeled_rectangle_text_field(
	const utki::shared_ref<ruis::context>& context, //
	ruis::labeled_rectangle_text_field::all_parameters params,
	ruis::string text
);
} // namespace make

} // namespace ruis
