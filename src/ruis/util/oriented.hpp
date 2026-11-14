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

#include <tuple>
#include <utility>

namespace ruis {

/**
 * @brief Base class for oriented widgets.
 * This is a base class for widgets which can be either vertical or horizontal, like lists or sliders.
 */
class oriented
{
public:
	/**
	 * @brief Oriented widget parameters.
	 */
	struct parameters {
		/**
		 * @brief Orientation flag.
		 * true for vertical orientation, false for horizontal one.
		 */
		bool vertical = true;
	};

private:
	parameters params;

protected:
	/**
	 * @brief Constructor.
	 * @param params - oriented widget parameters.
	 */
	oriented(parameters params) :
		params(std::move(params))
	{}

public:
	/**
	 * @brief Get index of the long (primary) dimension.
	 * @return 1 (y) if the widget is vertical, 0 (x) if it is horizontal.
	 */
	unsigned get_long_index() const noexcept
	{
		return this->params.vertical ? 1 : 0;
	}

	/**
	 * @brief Get index of the transverse (secondary) dimension.
	 * @return 0 (x) if the widget is vertical, 1 (y) if it is horizontal.
	 */
	unsigned get_trans_index() const noexcept
	{
		return this->params.vertical ? 0 : 1;
	}

	/**
	 * @brief Get indices of the longitudinal and transverse dimensions.
	 * @return pair of indices: (longitudinal index, transverse index).
	 */
	std::tuple<unsigned, unsigned> get_long_trans_indices() const noexcept
	{
		if (this->params.vertical) {
			return {1, 0};
		} else {
			return {0, 1};
		}
	}

	/**
	 * @brief Check if the widget is vertical.
	 * @return true if the widget is vertical, false if it is horizontal.
	 */
	bool is_vertical() const noexcept
	{
		return this->params.vertical;
	}
};

} // namespace ruis
