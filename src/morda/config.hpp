#pragma once

#include <utki/config.hpp>

#include <kolme/Matrix4.hpp>
#include <kolme/Vector4.hpp>
#include <kolme/Vector3.hpp>
#include <kolme/Vector2.hpp>
#include <kolme/Quaternion.hpp>
#include <kolme/Rectangle.hpp>

#include "util/Sides.hpp"


#define M_MORDA_RENDER_OPENGL				0
#define M_MORDA_RENDER_OPENGLES				1
#define M_MORDA_RENDER_DIRECTX				2
#define M_MORDA_RENDER_GCM					3


#if M_OS == M_OS_LINUX

#	if M_OS_NAME == M_OS_NAME_ANDROID
#		define M_MORDA_RENDER M_MORDA_RENDER_OPENGLES
#	else
#		define M_MORDA_RENDER M_MORDA_RENDER_OPENGL
#	endif

#elif M_OS == M_OS_WINDOWS

#	ifdef M_MORDA_ENFORCE_OPENGL_UNDER_WINDOWS
#		define M_MORDA_RENDER M_MORDA_RENDER_OPENGL
#	else
#		define M_MORDA_RENDER M_MORDA_RENDER_DIRECTX
#	endif

#elif M_OS == M_OS_MACOSX

#	if M_OS_NAME == M_OS_NAME_IOS
#		define M_MORDA_RENDER M_MORDA_RENDER_OPENGLES
#	else
#		define M_MORDA_RENDER M_MORDA_RENDER_OPENGL
#	endif

#else
#	error "unknown OS"
#endif



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

}//~namespace
