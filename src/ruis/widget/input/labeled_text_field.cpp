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

#include "labeled_text_field.hpp"

#include "impl/labeled_rectangle_text_field.hpp"

using namespace ruis;

labeled_text_field::labeled_text_field(
	const utki::shared_ref<ruis::context> context, //
	ruis::text_input& text_input,
	ruis::text& label
) :
	widget(context, {}, {}),
	labeled_widget(
		context, //
		label
	),
	text_field(
		context, //
		text_input
	)
{}

utki::shared_ref<ruis::labeled_text_field> ruis::make::labeled_text_field(
	const utki::shared_ref<ruis::context>& context, //
	ruis::labeled_text_field::all_parameters params,
	ruis::string text
)
{
	return ruis::make::labeled_rectangle_text_field(
		context, //
		// clang-format off
		{
			.layout_params = std::move(params.layout_params),
			.widget = std::move(params.widget),
			.params{
				.label = std::move(params.params.label),
				.rectangle_text_field{
					.text_input = std::move(params.params.text_input)
				}
			}
		},
		// clang-format on
		std::move(text)
	);
}
