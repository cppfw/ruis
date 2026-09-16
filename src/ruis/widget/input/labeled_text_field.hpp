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

#include "../base/labeled_widget.hpp"

#include "text_field.hpp"
#include "text_input.hpp"

namespace ruis {

class labeled_text_field :
	public labeled_widget, //
	public text_field
{
public:
	struct parameters {
		ruis::labeled_widget::parameters label;
		ruis::text_input::parameters text_input;
	};

	struct all_parameters {
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget;
		parameters params;
	};

protected:
	labeled_text_field(
		const utki::shared_ref<ruis::context> context, //
		ruis::text_input& text_input,
		ruis::text& label
	);
};

namespace make {

/**
 * @brief Factory function to create a labeled text field widget.
 * This creates a labeled_rectangle_text_field internally.
 *
 * @param context - GUI context.
 * @param params - Parameters for the labeled text field.
 * @param text - Initial text content.
 * @return Shared reference to the created widget.
 */
utki::shared_ref<ruis::labeled_text_field> labeled_text_field(
	const utki::shared_ref<ruis::context>& context, //
	ruis::labeled_text_field::all_parameters params,
	ruis::string text
);

} // namespace make

} // namespace ruis
