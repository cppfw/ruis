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

#include "easing.hpp"

using namespace ruis;

namespace {
constexpr auto half_way_factor = 0.5;
} // namespace

real ruis::easing::linear(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	return f;
}

real ruis::easing::in_sine(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using std::cos;
	return real(1) - cos(f * real(utki::pi) / 2);
}

real ruis::easing::out_sine(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using std::sin;
	return sin(f * real(utki::pi) / 2);
}

real ruis::easing::in_out_sine(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using std::cos;
	return -(cos(real(utki::pi) * f) - 1) / 2;
}

real ruis::easing::in_quadratic(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using utki::pow2;
	return pow2(f);
}

real ruis::easing::out_quadratic(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using utki::pow2;
	return real(1) - pow2(real(1) - f);
}

real ruis::easing::in_out_quadratic(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using utki::pow2;

	if (f < real(half_way_factor)) {
		return pow2(2 * f) / 2;
	} else {
		return real(1) - pow2(real(2) - 2 * f) / 2;
	}
}

real ruis::easing::in_cubic(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using utki::pow3;
	return pow3(f);
}

real ruis::easing::out_cubic(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using utki::pow3;
	return real(1) - pow3(real(1) - f);
}

real ruis::easing::in_out_cubic(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using utki::pow3;

	if (f < real(half_way_factor)) {
		return pow3(2 * f) / 2;
	} else {
		return real(1) - pow3(real(2) - 2 * f) / 2;
	}
}

real ruis::easing::in_quartic(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using utki::pow4;
	return pow4(f);
}

real ruis::easing::out_quartic(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using utki::pow4;
	return real(1) - pow4(real(1) - f);
}

real ruis::easing::in_out_quartic(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using utki::pow4;

	if (f < real(half_way_factor)) {
		return pow4(2 * f) / 2;
	} else {
		return real(1) - pow4(real(2) - 2 * f) / 2;
	}
}

real ruis::easing::in_quintic(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using utki::pow5;
	return pow5(f);
}

real ruis::easing::out_quintic(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using utki::pow5;
	return real(1) - pow5(real(1) - f);
}

real ruis::easing::in_out_quintic(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using utki::pow5;

	if (f < real(half_way_factor)) {
		return pow5(2 * f) / 2;
	} else {
		return real(1) - pow5(real(2) - 2 * f) / 2;
	}
}

namespace {
constexpr auto exponent_arg_infinity = 10;
} // namespace

real ruis::easing::in_exponent(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using std::pow;
	if (f == 0) {
		return 0;
	}
	return pow(real(2), exponent_arg_infinity * (f - 1));
}

real ruis::easing::out_exponent(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using std::pow;

	if (f == 1) {
		return 1;
	}
	return real(1) - pow(real(2), -exponent_arg_infinity * f);
}

real ruis::easing::in_out_exponent(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using std::pow;

	if (f == 0) {
		return 0;
	}
	if (f < half_way_factor) {
		return pow(real(2), exponent_arg_infinity * (f - 1)) / 2;
	}
	if (f < 1) {
		return (real(2) - pow(real(2), exponent_arg_infinity * (real(1) - f))) / 2;
	}
	return 1;
}
