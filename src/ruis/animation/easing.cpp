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
