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

#pragma once

#include <utki/config.hpp>

#include <r4/matrix.hpp>
#include <r4/vector.hpp>
#include <r4/quaternion.hpp>
#include <r4/rectangle.hpp>

#include "util/sides.hpp"

namespace morda{

typedef float real;

typedef r4::matrix4<real> matrix4;
static_assert(sizeof(matrix4) == sizeof(real) * 4 * 4, "size mismatch");

typedef r4::quaternion<real> quaternion;
static_assert(sizeof(quaternion) == sizeof(real) * 4, "size mismatch");

typedef r4::rectangle<real> rectangle;

typedef r4::vector2<real> vector2;
static_assert(sizeof(vector2) == sizeof(real) * 2, "size mismatch");

typedef r4::vector3<real> vector3;
static_assert(sizeof(vector3) == sizeof(real) * 3, "size mismatch");

typedef r4::vector4<real> vector4;
static_assert(sizeof(vector4) == sizeof(real) * 4, "size mismatch");

}
