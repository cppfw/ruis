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

namespace ruis::easing {

/**
 * @brief Ease linear.
 * Basically, this means no animation easing.
 * Function:
 *   F(f) = f
 * @param factor - factor from [0:1].
 * @return input factor.
 */
real linear(real factor);

/**
 * @brief Ease in sine.
 * Function:
 *   F(f) = 1 - cos(f * pi / 2);
 * @param factor - factor from [0:1].
 * @return sine-in eased factor from [0:1].
 */
real in_sine(real factor);

/**
 * @brief Ease out sine.
 * Function:
 *   F(f) = sin(f * pi / 2)
 * @param factor - factor from [0:1].
 * @return sine-out eased factor from [0:1].
 */
real out_sine(real factor);

/**
 * @brief Ease in-out sine.
 * Function:
 *   F(f) = -(cos(pi * f) - 1) / 2;
 * @param factor - factor from [0:1].
 * @return sine-in-out eased factor from [0:1].
 */
real in_out_sine(real factor);

/**
 * @brief Ease in quadratic.
 * Function:
 *   F(f) = x^2;
 * @param factor - factor from [0:1].
 * @return quadratic-in eased factor from [0:1].
 */
real in_quad(real factor);

/**
 * @brief Ease out quadratic.
 * Function:
 *   F(f) = 1 - (f - 1)^2
 * @param factor - factor from [0:1].
 * @return quadratic-out eased factor from [0:1].
 */
real out_quad(real factor);

/**
 * @brief Ease in-out quadratic.
 * Function:
 *   F(f) = | 2 * f^2, f < 0.5
 *          | 1 - 2 * (f - 1)^2, f >= 0.5
 * @param factor - factor from [0:1].
 * @return quadratic-in-out eased factor from [0:1].
 */
real in_out_quad(real factor);

// TODO: add other easings from https://easings.net

} // namespace ruis::easing
