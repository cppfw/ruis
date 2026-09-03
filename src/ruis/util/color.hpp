/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

/**
 * @brief Color class.
 * Represents a color value with 4 channels: red, green, blue, and alpha.
 * Each channel is an 8-bit unsigned integer (0-255).
 * Color channel indexing is:
 *   0 = red
 *   1 = green
 *   2 = blue
 *   3 = alpha
 *
 * A color with all four channels set to zero is considered "undefined"
 * and is used in styling to indicate that a default value should be applied.
 */
class color : public r4::vector4<uint8_t>
{
public:
	/**
	 * @brief Create a color from a 32-bit RGBA value.
	 * The 32-bit value is interpreted as follows:
	 * - bits 0-7: red component channel
	 * - bits 8-15: green component channel
	 * - bits 16-23: blue component channel
	 * - bits 24-31: alpha component channel
	 *
	 * The 0 value for all 4 channels means undefined value for the color.
	 * For a fully transparent color, use alpha = 0 and some other color component to be non-zero, e.g. red = 1.
	 *
	 * @param rgba - 32-bit RGBA value.
	 */
	constexpr color(uint32_t rgba = 0) :
		r4::vector4<uint8_t>(rasterimage::from_32bit_pixel(rgba))
	{}

	/**
	 * @brief Create a color from individual channel values.
	 * @param red - red channel (0-255).
	 * @param green - green channel (0-255).
	 * @param blue - blue channel (0-255).
	 * @param alpha - alpha channel (0-255).
	 */
	constexpr color(
		uint8_t red, //
		uint8_t green,
		uint8_t blue,
		uint8_t alpha)
	:
		r4::vector4<uint8_t>(red, green, blue, alpha)
	{}

	/**
	 * @brief Convert the color to a 32-bit RGBA value.
	 * @return The 32-bit RGBA representation of this color.
	 */
	constexpr uint32_t to_uint32_t() const noexcept
	{
		return rasterimage::to_32bit_pixel(*this);
	}

	/**
	 * @brief Convert the color to a 4D vector of floats.
	 * Each channel is normalized to the [0.0, 1.0] range.
	 * @return A 4D float vector representing the color in RGBA order.
	 */
	constexpr r4::vector4<float> to_vec4f() const noexcept
	{
		return rasterimage::to<float>(*this);
	}

	/**
	 * @brief Check if the color value is undefined.
	 * The color value is undefined when all 4 color components are set to 0.
	 * Though it is still a valid color, this particular value is thought of as undefined color
	 * for styling purposes.
	 * @return true if color value is undefined.
	 * @return false otherwise.
	 */
	constexpr bool is_undefined() const noexcept
	{
		return this->to_uint32_t() == 0;
	}

	/**
	 * @brief Create a color from a TML (tree markup language) forest descriptor.
	 * The descriptor format is a 32-bit unsigned integer in hex or decimal notation,
	 * representing the RGBA color value.
	 * The first value of the forest is taken and parsed as a 32-bit unsigned integer to create the color.
	 * @param desc - TML forest containing the color description.
	 * @return A color parsed from the descriptor.
	 */
	static color make_from(const tml::forest& desc);
};

} // namespace ruis
