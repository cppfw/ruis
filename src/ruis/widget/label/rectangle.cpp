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

#include "rectangle.hpp"

#include "../../context.hpp"
#include "../../util/util.hpp"

using namespace ruis;

rectangle::rectangle( //
	utki::shared_ref<ruis::context> context,
	all_parameters params,
	widget_list children
) :
	widget( //
		std::move(context),
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	margins(//
		this->context,
		{
			.container_params = std::move(params.container_params),
			.frame_params = std::move(params.frame_params)
		},
		std::move(children)
	),
	color_widget( //
		this->context,
		std::move(params.color_params)
	)
{}

void rectangle::render(const ruis::matrix4& matrix) const
{
	auto& r = this->context.get().renderer.get();
	r.render_context.get().set_simple_alpha_blending();

	ruis::matrix4 matr(matrix);
	matr.scale(this->rect().d);

	r.shaders().color_pos->render(
		matr, //
		r.obj().pos_quad_01_vao.get(),
		this->get_current_color().to_vec4f()
	);
}
