/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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
#include <r4/quaternion.hpp>
#include <r4/rectangle.hpp>
#include <r4/vector.hpp>
#include <utki/config.hpp>

#include "util/sides.hpp"

namespace ruis {

using real = float;

using mat4 = r4::matrix4<real>;
using matrix4 = mat4;
static_assert(sizeof(mat4) == sizeof(real) * 4 * 4, "size mismatch");

using quat = r4::quaternion<real>;
using quaternion = quat;
static_assert(sizeof(quat) == sizeof(real) * 4, "size mismatch");

using rect = r4::rectangle<real>;

using vec2 = r4::vector2<real>;
using vector2 = vec2;
static_assert(sizeof(vec2) == sizeof(real) * 2, "size mismatch");

using vec3 = r4::vector3<real>;
using vector3 = vec3;
static_assert(sizeof(vec3) == sizeof(real) * 3, "size mismatch");

using vec4 = r4::vector4<real>;
using vector4 = vec4;
static_assert(sizeof(vec4) == sizeof(real) * 4, "size mismatch");

} // namespace ruis
