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

#pragma once

#include "../config.hpp"

namespace ruis::render {

class texture_2d
{
	vector2 dims_v;

public:
	enum class filter {
		nearest,
		linear
	};

	enum class mipmap {
		none,
		nearest,
		linear
	};

protected:
	texture_2d(vector2 dims) :
		dims_v(dims)
	{}

public:
	texture_2d(const texture_2d&) = delete;
	texture_2d& operator=(const texture_2d&) = delete;

	texture_2d(texture_2d&&) = delete;
	texture_2d& operator=(texture_2d&&) = delete;

	virtual ~texture_2d() = default;

	const decltype(dims_v)& dims() const noexcept
	{
		return this->dims_v;
	}

	// TODO: add functions to change filtering
	// TODO: add virtual generate_mipmap()
};

} // namespace ruis::render
