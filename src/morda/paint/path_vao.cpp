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

#include "path_vao.hpp"

using namespace morda;

path_vao::path_vao(
	const utki::shared_ref<const morda::renderer>& r
) :
	renderer(r),
	core(this->renderer->empty_vertex_array),
	border(this->renderer->empty_vertex_array)
{}

void path_vao::set(const path::vertices& path){
	auto core_buf = this->renderer->factory->create_vertex_buffer(path.pos);

	this->core = this->renderer->factory->create_vertex_array(
			{
				core_buf,
			},
			this->renderer->factory->create_index_buffer(path.in_indices),
			morda::vertex_array::mode::triangle_strip
		);
	this->border = this->renderer->factory->create_vertex_array(
			{
				core_buf,
				this->renderer->factory->create_vertex_buffer(path.alpha),
			},
			this->renderer->factory->create_index_buffer(path.out_indices),
			morda::vertex_array::mode::triangle_strip
		);
}

void path_vao::render(const morda::matrix4& matrix, uint32_t color)const{
	this->renderer->shader->color_pos->render(matrix, *this->core, color);

	this->renderer->shader->color_pos_lum->render(matrix, *this->border, color);
}
