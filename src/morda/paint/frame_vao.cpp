/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

// NOLINTNEXTLINE(modernize-pass-by-value)
frame_vao::frame_vao(const utki::shared_ref<const morda::renderer>& r) :
	renderer(r),
	vao(this->renderer.get().empty_vertex_array)
{}

void frame_vao::set(vector2 dims, vector2 thickness)
{
	constexpr auto num_frame_vertices = 8;
	std::array<vector2, num_frame_vertices> vertices = {
		{// outer
{0, 0},
		 {dims.x(), 0},
		 dims, {0, dims.y()},

		 // inner
		 thickness, {dims.x() - thickness.x(), thickness.y()},
		 {dims.x() - thickness.x(), dims.y() - thickness.y()},
		 {thickness.x(), dims.y() - thickness.y()}}
    };

	// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
	std::array<uint16_t, 10> indices = {0, 4, 1, 5, 2, 6, 3, 7, 0, 4};

	this->vao = this->renderer.get().factory->create_vertex_array(
		{
			this->renderer.get().factory->create_vertex_buffer(vertices),
		},
		this->renderer.get().factory->create_index_buffer(indices),
		morda::vertex_array::mode::triangle_strip
	);
}

void frame_vao::render(const matrix4& matrix, uint32_t color) const
{
	this->renderer.get().shader->color_pos->render(matrix, this->vao.get(), color);
}
