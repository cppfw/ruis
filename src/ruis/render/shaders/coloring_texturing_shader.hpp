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

#pragma once

#include <r4/matrix.hpp>
#include <r4/vector.hpp>

#include "../texture_2d.hpp"
#include "../vertex_array.hpp"

namespace ruis::render {

class context;

class coloring_texturing_shader
{
protected:
	coloring_texturing_shader(utki::shared_ref<ruis::render::context> render_context) :
		render_context(std::move(render_context))
	{}

public:
	const utki::shared_ref<ruis::render::context> render_context;

	coloring_texturing_shader(const coloring_texturing_shader&) = delete;
	coloring_texturing_shader& operator=(const coloring_texturing_shader&) = delete;

	coloring_texturing_shader(coloring_texturing_shader&&) = delete;
	coloring_texturing_shader& operator=(coloring_texturing_shader&&) = delete;

	virtual ~coloring_texturing_shader() = default;

	virtual void render(
		const r4::matrix4<float>& m, //
		const ruis::render::vertex_array& va,
		const r4::vector4<float>& color,
		const ruis::render::texture_2d& tex
	) const = 0;
};

} // namespace ruis::render
