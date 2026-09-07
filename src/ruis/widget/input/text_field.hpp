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

#include "impl/rectangle_text_input_line.hpp"

namespace ruis {

namespace make {

/**
 * @brief Factory function to create a text field widget.
 * Creates a rectangle_text_input_line under the hood.
 * 
 * @param context GUI context
 * @param params Parameters for the text field
 * @param text Initial text content
 * @return Shared reference to the created decorated widget
 */
utki::shared_ref<ruis::decorated_widget<ruis::raw_text_input_line>> text_field(
	utki::shared_ref<ruis::context> context, //
	ruis::rectangle_text_input_line::all_parameters params,
	ruis::string text
);

} // namespace make

} // namespace ruis