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
constexpr real half_way_factor = 0.5;
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

	if (f < half_way_factor) {
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

	if (f < half_way_factor) {
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

	if (f < half_way_factor) {
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

	if (f < half_way_factor) {
		return pow5(2 * f) / 2;
	} else {
		return real(1) - pow5(real(2) - 2 * f) / 2;
	}
}

namespace {
constexpr real exponent_arg_infinity = 10;
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
		return pow(real(2), exponent_arg_infinity * (2 * f - 1)) / 2;
	}
	if (f < 1) {
		return (real(2) - pow(real(2), exponent_arg_infinity * (real(1) - 2 * f))) / 2;
	}
	return 1;
}

real ruis::easing::in_circular(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using utki::pow2;
	using std::sqrt;
	return real(1) - sqrt(real(1) - pow2(f));
}

real ruis::easing::out_circular(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using utki::pow2;
	using std::sqrt;
	return sqrt(real(1) - pow2(f - 1));
}

real ruis::easing::in_out_circular(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using utki::pow2;
	using std::sqrt;

	if (f < half_way_factor) {
		return (real(1) - sqrt(real(1) - pow2(2 * f))) / 2;
	} else {
		return (sqrt(real(1) - pow2(2 * (real(1) - f))) + 1) / 2;
	}
}

real ruis::easing::in_back(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using utki::pow2;
	using utki::pow3;

	constexpr real c1 = 1.70158;
	constexpr real c3 = c1 + 1;

	return c3 * pow3(f) - c1 * pow2(f);
}

real ruis::easing::out_back(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using utki::pow2;
	using utki::pow3;

	constexpr real c1 = 1.70158;
	constexpr real c3 = c1 + 1;

	return real(1) + c3 * pow3(f - 1) + c1 * pow2(f - 1);
}

real ruis::easing::in_out_back(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using utki::pow2;

	constexpr real c1 = 1.70158;
	constexpr real c2 = c1 * 1.525;

	if (f < half_way_factor) {
		return (pow2(2 * f) * (2 * (c2 + 1) * f - c2)) / 2;
	} else {
		return (pow2(2 * (f - 1)) * (2 * (c2 + 1) * (f - 1) + c2) + 2) / 2;
	}
}

real ruis::easing::in_elastic(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using std::pow;
	using std::sin;

	constexpr real c4 = (2 * utki::pi) / 3;
	constexpr real compress = 10;
	constexpr real sin_shift = 10.75;

	if (f == 0) {
		return 0;
	}
	if (f < 1) {
		return -pow(real(2), compress * (f - 1)) * sin((compress * f - sin_shift) * c4);
	}
	return 1;
}

real ruis::easing::out_elastic(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using std::pow;
	using std::sin;

	constexpr real c4 = (2 * utki::pi) / 3;
	constexpr real compress = 10;
	constexpr real sin_shift = 0.75;

	if (f == 0) {
		return 0;
	}
	if (f < 1) {
		return pow(real(2), -compress * f) * sin((compress * f - sin_shift) * c4) + 1;
	}
	return 1;
}

real ruis::easing::in_out_elastic(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	using std::pow;
	using std::sin;

	constexpr real c5 = (2 * utki::pi) / 4.5;
	constexpr real compress = 10;
	constexpr real sin_shift = 11.125;

	if (f == 0) {
		return 0;
	}
	if (f < half_way_factor) {
		return -pow(real(2), compress * (2 * f - 1)) * sin((2 * compress * f - sin_shift) * c5) / 2;
	}
	if (f < 1) {
		return pow(real(2), -compress * (2 * f - 1)) * sin((2 * compress * f - sin_shift) * c5) / 2 + 1;
	}
	return 1;
}

real ruis::easing::in_bounce(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)
	return real(1) - out_bounce(real(1) - f);
}

real ruis::easing::out_bounce(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)

	using utki::pow2;

	constexpr real n1 = 7.5625;
	constexpr real d1 = 2.75;

	if (f < real(1) / d1) {
		return n1 * pow2(f);
	}
	if (f < 2 / d1) {
		constexpr real a = 1.5;
		constexpr real b = 0.75;
		return n1 * pow2(f - a / d1) + b;
	}
	if (constexpr real threshold = 2.5; f < threshold / d1) {
		constexpr real a = 2.25;
		constexpr real b = 0.9375;
		return n1 * pow2(f - a / d1) + b;
	}

	constexpr real a = 2.625;
	constexpr real b = 0.984375;
	return n1 * pow2(f - a / d1) + b;
}

real ruis::easing::in_out_bounce(real f)
{
	ASSERT(f >= 0)
	ASSERT(f <= 1)

	if (f < half_way_factor) {
		return (1 - out_bounce(1 - 2 * f)) / 2;
	} else {
		return (1 + out_bounce(2 * f - 1)) / 2;
	}
}
