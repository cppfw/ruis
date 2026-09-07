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

#include "text_field.hpp"

#include "impl/rectangle_text_input_line.hpp"

utki::shared_ref<ruis::decorated_widget<ruis::raw_text_input_line>> ruis::make::text_field(
	utki::shared_ref<ruis::context> context, //
	ruis::rectangle_text_input_line::all_parameters params,
	ruis::string text
)
{
	auto ret = ruis::make::rectangle_text_input_line(
		std::move(context), //
		std::move(params),
		std::move(text)
	);
	return ret;
}