#pragma once

#include <utki/config.hpp>

#include <r4/matrix4.hpp>
#include <r4/vector4.hpp>
#include <r4/vector3.hpp>
#include <r4/vector2.hpp>
#include <r4/quaternion.hpp>
#include <r4/rectangle.hpp>

#include "util/sides.hpp"

namespace morda{

typedef float real;

typedef r4::matrix4<real> matrix4;
static_assert(sizeof(matrix4) == sizeof(real) * 4 * 4, "size mismatch");

//TODO: deprecated, remove.
typedef matrix4 Matr4r;

typedef r4::quaternion<real> quaternion;
static_assert(sizeof(quaternion) == sizeof(real) * 4, "size mismatch");

//TODO: deprecated, remove.
typedef quaternion Quatr;

typedef r4::rectangle<real> rectangle;

//TODO: deprecated, remove.
typedef rectangle Rectr;

typedef r4::vector2<real> vector2;
static_assert(sizeof(vector2) == sizeof(real) * 2, "size mismatch");

//TODO: deprecated, remove.
typedef vector2 Vec2r;

typedef r4::vector3<real> vector3;
static_assert(sizeof(vector3) == sizeof(real) * 3, "size mismatch");

//TODO: deprecated, remove.
typedef vector3 Vec3r;

typedef r4::vector4<real> vector4;
static_assert(sizeof(vector4) == sizeof(real) * 4, "size mismatch");

//TODO: deprecated, remove.
typedef vector4 Vec4r;

//TODO: deprecated, remove.
typedef sides<real> Sidesr;

}
