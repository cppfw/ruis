#pragma once

#include <utki/config.hpp>

#include <r4/matrix4.hpp>
#include <r4/vector4.hpp>
#include <r4/vector3.hpp>
#include <r4/vector2.hpp>
#include <r4/quaternion.hpp>
#include <r4/rectangle.hpp>

#include "util/Sides.hpp"



namespace morda{

typedef float real;

typedef r4::matrix4<real> Matr4r;
static_assert(sizeof(Matr4r) == sizeof(real) * 4 * 4, "size mismatch");

typedef r4::quaternion<real> Quatr;
static_assert(sizeof(Quatr) == sizeof(real) * 4, "size mismatch");

typedef r4::rectangle<real> Rectr;

typedef r4::vector2<real> Vec2r;
static_assert(sizeof(Vec2r) == sizeof(real) * 2, "size mismatch");

typedef r4::vector3<real> Vec3r;
static_assert(sizeof(Vec3r) == sizeof(real) * 3, "size mismatch");

typedef r4::vector4<real> Vec4r;
static_assert(sizeof(Vec4r) == sizeof(real) * 4, "size mismatch");

typedef Sides<real> Sidesr;

}
