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

#include "../base/wrapped_widget.hpp"

#include "bare_text_field.hpp"

namespace ruis {

// TODO: add doxygen docs
class text_field : public wrapped_widget<bare_text_field>
{
public:
	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
		color_widget::parameters color_params;
		text_widget::parameters text_widget_params;
		bare_text_field::parameters text_field_params;
	};

protected:
	text_field(
		const utki::shared_ref<ruis::context>& context, //
		bare_text_field& bare
	);
};

namespace make {

/**
 * @brief Factory function to create a text field widget.
 * 
 * @param context - GUI context.
 * @param params - Parameters for the text field.
 * @param text - Initial text content.
 * @return Shared reference to the created widget.
 */
utki::shared_ref<ruis::text_field> text_field(
	const utki::shared_ref<ruis::context>& context, //
	ruis::text_field::all_parameters params,
	ruis::string text
);

} // namespace make

} // namespace ruis