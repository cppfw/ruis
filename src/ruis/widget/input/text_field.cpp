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

#include "impl/rectangle_text_field.hpp"

using namespace ruis;

text_field::text_field(
	const utki::shared_ref<ruis::context>& context, //
	ruis::text_input& text_input
) :
	widget(context, {}, {}),
	text_input(text_input)
{}

utki::shared_ref<ruis::text_field> ruis::make::text_field(
	const utki::shared_ref<ruis::context>& context, //
	ruis::text_field::all_parameters params,
	ruis::string text
)
{
	return ruis::make::rectangle_text_field(
		context, //
		// clang-format off
		{
			.layout_params = std::move(params.layout_params),
			.widget_params = std::move(params.widget_params),
			.params{
				.text_input_params = std::move(params.text_input_params)
			}
		},
		// clang-format on
		std::move(text)
	);
}