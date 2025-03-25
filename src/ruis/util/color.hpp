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
	color(uint32_t rgba) :
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

	static color parse_style_value(const tml::forest& desc);

	static color default_value() noexcept
	{
		constexpr auto default_color_value = 0xffffffff;
		return default_color_value;
	}
};

} // namespace ruis
