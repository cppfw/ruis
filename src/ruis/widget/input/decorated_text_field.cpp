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

#include "decorated_text_field.hpp"

#include "impl/rectangle_text_field.hpp"

utki::shared_ref<ruis::wrapped_widget<ruis::bare_text_field>> ruis::make::decorated_text_field(
	utki::shared_ref<ruis::context> context, //
	ruis::rectangle_text_field::all_parameters params,
	ruis::string text
)
{
	auto ret = ruis::make::rectangle_text_field(
		std::move(context), //
		// clang-format off
		{
			.layout_params = std::move(params.layout_params),
			.widget_params = std::move(params.widget_params),
			.color_params = std::move(params.color_params),
			.text_widget_params = std::move(params.text_widget_params),
			.text_field_params = std::move(params.text_field_params)
		},
		// clang-format on
		std::move(text)
	);
	return ret;
}