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

#include "vertex_array.hpp"

namespace morda {

class shader
{
public:
	shader() = default;

	shader(const shader&) = delete;
	shader& operator=(const shader&) = delete;

	shader(shader&&) = delete;
	shader& operator=(shader&&) = delete;

	virtual ~shader() = default;

	virtual void render(const r4::matrix4<float>& m, const vertex_array& va) const = 0;
};

} // namespace morda
