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

#include <ratio>

#include "../config.hpp"

namespace ruis {
/**
 * @brief Information about screen units.
 * This class holds information about screen units and performs conversion
 * from one unit to another.
 * Length can be expressed in pixels, millimeters or perception pixels.
 * Perception pixel is a convenience unit which is different depending on the screen dimensions,
 * density and supposed viewing distance (e.g. large TV is viewed from 2-3 meters, and tablet is viewed from 30-50cm).
 * Perception pixel is never less than one pixel.
 * For normal desktop displays like HD or Full HD point is equal to one pixel.
 * For higher density desktop displays point is more than one pixel depending on density.
 * For mobile platforms the point is also 1 or more pixels depending on display density and physical size.
 */
class units
{
	real dots_per_inch_v;

	/**
	 * @brief Dots per perception pixel.
	 */
	real dots_per_pp_v;

	/**
	 * @brief Dots per font pixel.
	 */
	real dots_per_fp_v; // TODO: is needed?

public:
	/**
	 * @brief Constructor.
	 * @param dots_per_inch - dots per inch.
	 * @param dots_per_pp - dots per perception pixel.
	 */
	units(real dots_per_inch, real dots_per_pp) :
		dots_per_inch_v(dots_per_inch),
		dots_per_pp_v(dots_per_pp),
		dots_per_fp_v(dots_per_pp) // TODO: for now same as pp
	{}

	void set_dots_per_pp(real dots_per_pp)
	{
		this->dots_per_pp_v = dots_per_pp;
	}

	void set_dots_per_inch(real dpi)
	{
		this->dots_per_inch_v = dpi;
	}

	/**
	 * @brief Get dots (pixels) per perception pixel.
	 * @return Dots per perception pixel.
	 */
	real dots_per_pp() const noexcept
	{
		return this->dots_per_pp_v;
	}

	/**
	 * @brief Get dots (pixels) per font pixel.
	 * @return Dots per font pixel.
	 */
	real dots_per_fp() const noexcept
	{
		return this->dots_per_fp_v;
	}

	/**
	 * @brief Get dots (pixels) per inch.
	 * @return Dots per inch.
	 */
	real dots_per_inch() const noexcept
	{
		return this->dots_per_inch_v;
	}

	/**
	 * @brief Get dots (pixels) per centimeter.
	 * @return Dots per centimeter.
	 */
	real dots_per_cm() const noexcept
	{
		constexpr auto num_cm_in_inch = 2.54; // TODO: use constant from utki
		return this->dots_per_inch() / real(num_cm_in_inch);
	}

	/**
	 * @brief Convert millimeters to pixels (dots).
	 * @param mm - value in millimeters.
	 * @return Value in pixels.
	 */
	real mm_to_px(real mm) const noexcept
	{
		using std::round;
		return round(mm * this->dots_per_cm() / real(std::deci::den));
	}

	/**
	 * @brief Convert perception pixels to pixels.
	 * @param pp - value in perception pixels.
	 * @return  Value in pixels.
	 */
	real pp_to_px(real pp) const noexcept
	{
		using std::round;
		return round(pp * this->dots_per_pp());
	}
};

} // namespace ruis
