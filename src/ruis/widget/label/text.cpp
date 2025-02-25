/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include "text.hpp"

#include "../../context.hpp"
#include "../../util/util.hpp"

using namespace ruis;

text::text(
	utki::shared_ref<ruis::context> context, //
	all_parameters params,
	string text
) :
	widget(
		std::move(context), //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	text_string_widget(
		this->context, //
		std::move(params.text_params),
		std::move(text)
	),
	color_widget(
		this->context, //
		std::move(params.color_params)
	)
{}

void text::render(const ruis::matrix4& matrix) const
{
	ruis::matrix4 matr(matrix);

	using std::round;

	const auto& font = this->get_font();

	matr.translate(
		-this->get_bounding_box().p.x(), //
		font.get_ascender()
	);

	font.render(
		matr, //
		ruis::color_to_vec4f(this->get_current_color()),
		this->get_string()
	);
}
