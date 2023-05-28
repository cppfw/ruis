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

#include "texture_2d.hpp"
#include "vertex_array.hpp"

namespace morda {

class texturing_shader
{
public:
	texturing_shader() = default;

	texturing_shader(const texturing_shader&) = delete;
	texturing_shader& operator=(const texturing_shader&) = delete;

	virtual ~texturing_shader() = default;

	virtual void render(const r4::matrix4<float>& m, const morda::vertex_array& va, const texture_2d& tex) const = 0;

private:
};

} // namespace morda
