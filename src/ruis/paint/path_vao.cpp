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

#include "path_vao.hpp"

using namespace ruis;

// NOLINTNEXTLINE(modernize-pass-by-value)
path_vao::path_vao(const utki::shared_ref<const ruis::render::renderer>& r) :
	renderer(r),
	core(this->renderer.get().obj().empty_vertex_array),
	border(this->renderer.get().obj().empty_vertex_array)
{}

void path_vao::set(const path::vertices& path)
{
	auto core_buf = this->renderer.get().rendering_context.get().make_vertex_buffer(path.pos);

	this->core = this->renderer.get().rendering_context.get().make_vertex_array(
		{
			core_buf,
		},
		this->renderer.get().rendering_context.get().make_index_buffer(path.in_indices),
		ruis::render::vertex_array::mode::triangle_strip
	);
	this->border = this->renderer.get().rendering_context.get().make_vertex_array(
		{
			core_buf,
			this->renderer.get().rendering_context.get().make_vertex_buffer(path.alpha),
		},
		this->renderer.get().rendering_context.get().make_index_buffer(path.out_indices),
		ruis::render::vertex_array::mode::triangle_strip
	);
}

void path_vao::render(
	const ruis::matrix4& matrix, //
	ruis::color color
) const
{
	this->renderer.get().shaders().color_pos->render(
		matrix, //
		this->core.get(),
		color
	);

	this->renderer.get().shaders().color_pos_lum->render(
		matrix, //
		this->border.get(),
		color
	);
}
