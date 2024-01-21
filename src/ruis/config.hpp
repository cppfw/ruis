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
#include <r4/quaternion.hpp>
#include <r4/rectangle.hpp>
#include <r4/vector.hpp>
#include <utki/config.hpp>

#include "util/sides.hpp"

namespace morda {

using real = float;

using matrix4 = r4::matrix4<real>;
static_assert(sizeof(matrix4) == sizeof(real) * 4 * 4, "size mismatch");

using quaternion = r4::quaternion<real>;
static_assert(sizeof(quaternion) == sizeof(real) * 4, "size mismatch");

using rectangle = r4::rectangle<real>;

using vector2 = r4::vector2<real>;
static_assert(sizeof(vector2) == sizeof(real) * 2, "size mismatch");

using vector3 = r4::vector3<real>;
static_assert(sizeof(vector3) == sizeof(real) * 3, "size mismatch");

using vector4 = r4::vector4<real>;
static_assert(sizeof(vector4) == sizeof(real) * 4, "size mismatch");

} // namespace morda
