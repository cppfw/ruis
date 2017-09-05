#pragma once

#include <utki/config.hpp>

#include <kolme/Matrix4.hpp>
#include <kolme/Vector4.hpp>
#include <kolme/Vector3.hpp>
#include <kolme/Vector2.hpp>
#include <kolme/Quaternion.hpp>
#include <kolme/Rectangle.hpp>

#include "util/Sides.hpp"



namespace morda{

typedef float real;

typedef kolme::Matrix4<real> Matr4r;
static_assert(sizeof(Matr4r) == sizeof(real) * 4 * 4, "size mismatch");

typedef kolme::Quaternion<real> Quatr;
static_assert(sizeof(Quatr) == sizeof(real) * 4, "size mismatch");

typedef kolme::Rectangle<real> Rectr;

typedef kolme::Vector2<real> Vec2r;
static_assert(sizeof(Vec2r) == sizeof(real) * 2, "size mismatch");

typedef kolme::Vector3<real> Vec3r;
static_assert(sizeof(Vec3r) == sizeof(real) * 3, "size mismatch");

typedef kolme::Vector4<real> Vec4r;
static_assert(sizeof(Vec4r) == sizeof(real) * 4, "size mismatch");

typedef Sides<real> Sidesr;

}
