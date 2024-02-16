/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

rectangle::rectangle(
	utki::shared_ref<ruis::context> context,
	widget::parameters widget_params,
	color_widget::parameters color_widget_params
) :
	widget(std::move(context), std::move(widget_params)),
	color_widget(this->context, std::move(color_widget_params))
{}

void rectangle::render(const ruis::matrix4& matrix) const
{
	auto& r = this->context.get().renderer.get();
	r.set_simple_alpha_blending();

	ruis::matrix4 matr(matrix);
	matr.scale(this->rect().d);

	r.shader->color_pos->render(matr, r.pos_quad_01_vao.get(), this->get_current_color());
}
