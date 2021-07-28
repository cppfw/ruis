/*
morda GUI framework

Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

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

#include "color.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"

using namespace morda;

color::color(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		color_widget(this->context, desc)
{}

void color::render(const morda::matrix4& matrix)const{
	auto& r = *this->context->renderer;
	set_simple_alpha_blending(r);
	
	morda::matrix4 matr(matrix);
	matr.scale(this->rect().d);

	r.shader->color_pos->render(matr, *r.pos_quad_01_vao, this->get_current_color());
}
