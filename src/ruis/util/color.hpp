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

#include <cstdint>

#include <rasterimage/operations.hpp>
#include <tml/tree.hpp>

namespace ruis {

class color : public r4::vector4<uint8_t>
{
public:
	color(uint32_t rgba = 0) :
		r4::vector4<uint8_t>(rasterimage::from_32bit_pixel(rgba))
	{}

	uint32_t to_uint32_t() const noexcept
	{
		return rasterimage::to_32bit_pixel(*this);
	}

	r4::vector4<float> to_vec4f() const noexcept
	{
		return rasterimage::to<float>(*this);
	}

	/**
	 * @brief Check if the color value is undefined.
	 * The color value is undefined when all 4 color components are set to 0.
	 * Though it is still a valid color, this particular value is thought of as undefined color
	 * for styling purposes.
	 * @return true if color value is undefined.
	 * @return false oterwise.
	 */
	bool is_undefined() const noexcept
	{
		return this->to_uint32_t() == 0;
	}

	static color make_from(const tml::forest& desc);
};

} // namespace ruis
