/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include <rasterimage/dimensioned.hpp>

namespace ruis::render {

class texture_depth : public rasterimage::dimensioned
{
protected:
	texture_depth(r4::vector2<uint32_t> dims) :
		rasterimage::dimensioned(dims)
	{}

public:
	texture_depth(const texture_depth&) = delete;
	texture_depth& operator=(const texture_depth&) = delete;

	texture_depth(texture_depth&&) = delete;
	texture_depth& operator=(texture_depth&&) = delete;

	virtual ~texture_depth() = default;
};

} // namespace ruis::render
