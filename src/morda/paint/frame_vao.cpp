/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

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

#include "frame_vao.hpp"

using namespace morda;

frame_vao::frame_vao(std::shared_ptr<morda::renderer> r, vector2 dims, vector2 thickness) :
		renderer(std::move(r))
{
	std::array<vector2, 8> vertices = {{
		// outer
		{0, 0},
		{dims.x(), 0},
		dims,
		{0, dims.y()},

		// inner
		thickness,
		{dims.x() - thickness.x(), thickness.y()},
		{dims.x() - thickness.x(), dims.y() - thickness.y()},
		{thickness.x(), dims.y() - thickness.y()}
	}};

	std::array<uint16_t, 10> indices = {{
		4, 0, 1, 5, 2, 6, 3, 7, 4, 0
	}};

	auto vbo = this->renderer->factory->create_vertex_buffer(vertices);
	
	this->vao = this->renderer->factory->create_vertex_array(
			{
				vbo,
			},
			this->renderer->factory->create_index_buffer(indices),
			morda::vertex_array::mode::triangle_strip
		);
}

void frame_vao::render(const matrix4& matrix, uint32_t color)const{
	if(!this->renderer || ! this->vao){
		return;
	}

	this->renderer->shader->color_pos->render(matrix, *this->vao, color);
}
