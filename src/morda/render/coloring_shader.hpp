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

#pragma once

#include <r4/matrix.hpp>

#include "../util/util.hpp"

#include "vertex_array.hpp"

namespace morda {

class coloring_shader
{
public:
	coloring_shader() = default;

	coloring_shader(const coloring_shader&) = delete;
	coloring_shader& operator=(const coloring_shader&) = delete;

	coloring_shader(coloring_shader&&) = delete;
	coloring_shader& operator=(coloring_shader&&) = delete;

	virtual ~coloring_shader() = default;

	virtual void render(const r4::matrix4<float>& m, const vertex_array& va, r4::vector4<float> color) const = 0;

	void render(const r4::matrix4<float>& m, const vertex_array& va, uint32_t color) const
	{
		this->render(m, va, color_to_vec4f(color));
	}
};

} // namespace morda
