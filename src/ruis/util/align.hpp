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

#include <tml/tree.hpp>

namespace ruis {

/**
 * @brief Alignment parameter values.
 */
enum class align {
	/**
	 * @brief Undefined alignment.
	 * In case alignemnt is undefined the default center alignment is assumed.
	 */
	undefined,

	/**
	 * @brief Left or top alignment.
	 */
	front,

	/**
	 * @brief Center lignment.
	 */
	center,

	/**
	 * @brief Right or bottom alignment.
	 */
	back
};

/**
 * TODO:
 */
align make_from(const tml::forest& desc, const align& adl);

} // namespace ruis
