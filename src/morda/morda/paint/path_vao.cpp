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

#include "path_vao.hpp"

using namespace morda;

path_vao::path_vao(std::shared_ptr<morda::renderer> r, const path::vertices& path) :
		renderer(std::move(r))
{
	auto coreBuf = this->renderer->factory->create_vertex_buffer(utki::make_span(path.pos));
	
	this->core = this->renderer->factory->create_vertex_array(
			{{
				coreBuf,
			}},
			this->renderer->factory->create_index_buffer(utki::make_span(path.in_indices)),
			morda::vertex_array::mode::triangle_strip
		);
	
	
	this->border = this->renderer->factory->create_vertex_array(
			{{
				coreBuf,
				this->renderer->factory->create_vertex_buffer(utki::make_span(path.alpha)),
			}},
			this->renderer->factory->create_index_buffer(utki::make_span(path.out_indices)),
			morda::vertex_array::mode::triangle_strip
		);
}

void path_vao::render(const morda::matrix4& matrix, uint32_t color)const{
	if(!this->renderer || ! this->core){
		return;
	}

	this->renderer->shader->color_pos->render(matrix, *this->core, color);

	if(this->border){
		this->renderer->shader->color_pos_lum->render(matrix, *this->border, color);
	}
}
