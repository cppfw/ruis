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
	virtual public widget, //
	private container,
	public labeled_text_field
{
public:
	struct parameters {
		ruis::labeled_widget::parameters label;
		ruis::rectangle_text_field::parameters rectangle_text_field;
	};

	struct all_parameters {
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget;
		parameters params;
	};

public:
	labeled_rectangle_text_field(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params,
		ruis::string text
	);

private:
	// Private constructor used by the public one to pass the already-constructed
	// label and text field children to the labeled_text_field base. This ensures the
	// labeled_text_field base (and its text_field / labeled_widget sub-objects) receive
	// valid references instead of reading them before they are initialized.
	labeled_rectangle_text_field(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters& params,
		utki::shared_ref<ruis::text> label,
		utki::shared_ref<ruis::rectangle_text_field> text_field
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
