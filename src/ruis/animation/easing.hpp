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
 *   F(f) = 1 - cos(f * pi / 2)
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
 *   F(f) = -(cos(pi * f) - 1) / 2
 * @param factor - factor from [0:1].
 * @return sine-in-out eased factor from [0:1].
 */
real in_out_sine(real factor);

/**
 * @brief Ease in quadratic.
 * Function:
 *   F(f) = f^2
 * @param factor - factor from [0:1].
 * @return quadratic-in eased factor from [0:1].
 */
real in_quadratic(real factor);

/**
 * @brief Ease out quadratic.
 * Function:
 *   F(f) = 1 - (1 - f)^2
 * @param factor - factor from [0:1].
 * @return quadratic-out eased factor from [0:1].
 */
real out_quadratic(real factor);

/**
 * @brief Ease in-out quadratic.
 * Function:
 *   F(f) = | (2 * f)^2 / 2, f < 0.5
 *          | 1 - (2 - 2 * f)^2 / 2, f >= 0.5
 * @param factor - factor from [0:1].
 * @return quadratic-in-out eased factor from [0:1].
 */
real in_out_quadratic(real factor);

/**
 * @brief Ease in cubic.
 * Function:
 *   F(f) = f^3
 * @param factor - factor from [0:1].
 * @return cubic-in eased factor from [0:1].
 */
real in_cubic(real factor);

/**
 * @brief Ease out cubic.
 * Function:
 *   F(f) = 1 - (1 - f)^3
 * @param factor - factor from [0:1].
 * @return cubic-out eased factor from [0:1].
 */
real out_cubic(real factor);

/**
 * @brief Ease in-out cubic.
 * Function:
 *   F(f) = | (2 * f)^3 / 2, f < 0.5
 *          | 1 - (2 - 2 * f)^3 / 2, f >= 0.5
 * @param factor - factor from [0:1].
 * @return cubic-in-out eased factor from [0:1].
 */
real in_out_cubic(real factor);

/**
 * @brief Ease in quartic.
 * Function:
 *   F(f) = f^4
 * @param factor - factor from [0:1].
 * @return quartic-in eased factor from [0:1].
 */
real in_quartic(real factor);

/**
 * @brief Ease out quartic.
 * Function:
 *   F(f) = 1 - (1 - f)^4
 * @param factor - factor from [0:1].
 * @return quartic-out eased factor from [0:1].
 */
real out_quartic(real factor);

/**
 * @brief Ease in-out quartic.
 * Function:
 *   F(f) = | (2 * f)^4 / 2, f < 0.5
 *          | 1 - (2 - 2 * f)^4 / 2, f >= 0.5
 * @param factor - factor from [0:1].
 * @return quartic-in-out eased factor from [0:1].
 */
real in_out_quartic(real factor);

/**
 * @brief Ease in quintic.
 * Function:
 *   F(f) = f^5
 * @param factor - factor from [0:1].
 * @return quintic-in eased factor from [0:1].
 */
real in_quintic(real factor);

/**
 * @brief Ease out quintic.
 * Function:
 *   F(f) = 1 - (1 - f)^5
 * @param factor - factor from [0:1].
 * @return quintic-out eased factor from [0:1].
 */
real out_quintic(real factor);

/**
 * @brief Ease in-out quintic.
 * Function:
 *   F(f) = | (2 * f)^5 / 2, f < 0.5
 *          | 1 - (2 - 2 * f)^5 / 2, f >= 0.5
 * @param factor - factor from [0:1].
 * @return quintic-in-out eased factor from [0:1].
 */
real in_out_quintic(real factor);

/**
 * @brief Ease in exponent.
 * Function:
 *   F(f) = | 0, f = 0
 *          | 2^(10 * (f - 1)), f > 0
 * @param factor - factor from [0:1].
 * @return exponent-in eased factor from [0:1].
 */
real in_exponent(real factor);

/**
 * @brief Ease out exponent.
 * Function:
 *   F(f) = | 1 - 2^(-10 * f), f < 1
 *          | 1, f = 1
 * @param factor - factor from [0:1].
 * @return exponent-out eased factor from [0:1].
 */
real out_exponent(real factor);

/**
 * @brief Ease in-out exponent.
 * Function:
 *   F(f) = | 0, f = 0
 *          | 2^(10 * (f - 1)) / 2, f < 0.5
 *          | (2 - 2^(10 * (1 - f))) / 2, f >= 0.5
 *          | 1, f = 1
 * @param factor - factor from [0:1].
 * @return exponent-in-out eased factor from [0:1].
 */
real in_out_exponent(real factor);

/**
 * @brief Ease in circular.
 * Function:
 *   F(f) = 1 - sqrt(1 - f^2)
 * @param factor - factor from [0:1].
 * @return circular-in eased factor from [0:1].
 */
real in_circular(real factor);

/**
 * @brief Ease out circular.
 * Function:
 *   F(f) = sqrt(1 - (f - 1)^2)
 * @param factor - factor from [0:1].
 * @return circular-out eased factor from [0:1].
 */
real out_circular(real factor);

/**
 * @brief Ease in-out circular.
 * Function:
 *   F(f) = | (1 - sqrt(1 - (2 * f)^2)) / 2, f < 0.5
 *          | sqrt(1 - (2 * (1 - f))^2) / 2, f >= 0.5
 * @param factor - factor from [0:1].
 * @return circular-in-out eased factor from [0:1].
 */
real in_out_circular(real factor);

/**
 * @brief Ease in back.
 * Function:
 *   c1 = 1.70158
 *   c3 = c1 + 1
 *   F(f) = c3 * f^3 - c1 * f^2
 * @param factor - factor from [0:1].
 * @return back-in eased factor from [0:1].
 */
real in_back(real factor);

/**
 * @brief Ease out back.
 * Function:
 *   c1 = 1.70158
 *   c3 = c1 + 1
 *   F(f) = 1 + c3 * (f - 1)^3 + c1 * (f - 1)^2
 * @param factor - factor from [0:1].
 * @return back-out eased factor from [0:1].
 */
real out_back(real factor);

/**
 * @brief Ease in-out back.
 * Function:
 *   c1 = 1.70158
 *   c2 = c1 * 1.525
 *   F(f) = | ((2 * f)^2 * (2 * (c2 + 1) * f - c2)) / 2, f < 0.5
 *          | ((2 * (f - 1))^2 * (2 * (c2 + 1) * (f - 1) + c2) + 2) / 2, f >= 0.5
 * @param factor - factor from [0:1].
 * @return back-in-out eased factor from [0:1].
 */
real in_out_back(real factor);

// TODO: add other easings from https://easings.net

} // namespace ruis::easing
