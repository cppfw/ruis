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

#include "../config.hpp"
#include "../style/styled.hpp"
#include "../util/align.hpp"

#include "dimension.hpp"

namespace ruis::layout {

/**
 * @brief Widget layout parameters.
 */
struct parameters {
	/**
	 * @brief desired dimensions.
	 */
	r4::vector2<styled<dimension>> dims;

	/**
	 * @brief Weight of the widget.
	 * Weight defines how much space widget occupies in addition to its minimal or explicitly set size.
	 * Default weight is 0, which means that the widget will not occupy extra space.
	 * Value less than 0 is invalid, default weight will be assumed.
	 */
	styled<real> weight = -1;

	/**
	 * @brief Alignment of the widget within its parent.
	 * Horizontal and vertical alignment.
	 */
	r4::vector2<styled<ruis::align>> align = {ruis::align::undefined, ruis::align::undefined};
};

}; // namespace ruis::layout
