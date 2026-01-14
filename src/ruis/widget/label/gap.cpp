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

#include "gap.hpp"

using namespace ruis;

gap::gap(
	utki::shared_ref<ruis::context> context,
	all_parameters params //
) :
	widget(
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params) //
	),
	color_widget(
		this->context,
		std::move(params.color_params) //
	)
{}

void gap::render(const mat4& matrix) const
{
	if (this->get_current_color().is_undefined()) {
		return;
	}

	// draw rectangle
	auto& r = this->context.get().renderer.get();

	ruis::matrix4 matr(matrix);
	matr.scale(this->rect().d);

	r.shaders().color_pos->render(
		matr, //
		r.obj().pos_quad_01_vao.get(),
		this->get_current_color().to_vec4f()
	);
}
