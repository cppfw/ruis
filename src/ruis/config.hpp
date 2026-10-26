/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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
static_assert(sizeof(mat4) == sizeof(real) * 4 * 4, "size mismatch");

using mat3 = r4::matrix3<real>;
static_assert(sizeof(mat3) == sizeof(real) * 3 * 3, "size mismatch");

using quat = r4::quaternion<real>;
static_assert(sizeof(quat) == sizeof(real) * 4, "size mismatch");

using rect = r4::rectangle<real>;

using vec2 = r4::vector2<real>;
static_assert(sizeof(vec2) == sizeof(real) * 2, "size mismatch");

using vec3 = r4::vector3<real>;
static_assert(sizeof(vec3) == sizeof(real) * 3, "size mismatch");

using vec4 = r4::vector4<real>;
static_assert(sizeof(vec4) == sizeof(real) * 4, "size mismatch");

/**
 * @brief A "very large" length, in pixels.
 * This value is bigger than any reasonable screen or viewport dimension, even for
 * content which is much longer than the screen (e.g. long scrollable contents of
 * scroll areas or lists). It is used as a generic "infinite" length in situations
 * where a value is only required to be larger than anything the user can see, for
 * example to seed a rectangle's corner radii (see normalize_corner_radii()) so that
 * the rectangle becomes a capsule (stadium) shape after normalization.
 *
 * It is intentionally not set to std::numeric_limits<real>::max() so that it can be
 * safely used in arithmetic (e.g. computing an average of corner radii) without
 * causing overflow or undefined behavior.
 */
constexpr real big_length_px = 10000000; // 10^7

} // namespace ruis
