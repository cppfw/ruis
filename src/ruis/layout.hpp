/*
ruis - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#include "util/widget_list.hpp"

#include "config.hpp"

namespace ruis {

// TODO: doxygen
class layout
{
protected:
	layout() = default;

public:
	layout(const layout&) = delete;
	layout& operator=(const layout&) = delete;

	layout(layout&&) = delete;
	layout& operator=(layout&&) = delete;

	virtual vector2 measure(const vector2& quotum, const_widget_list& widgets) const = 0;

	/**
	 * @brief Arrange widgets.
	 * @param size - size of the area available to the layout.
	 * @param widgets - widgets to arrange.
	 */
	virtual void lay_out(const vector2& size, semiconst_widget_list& widgets) const = 0;

	virtual ~layout() = default;
};

} // namespace ruis
