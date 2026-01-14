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
#include "../util/widget_list.hpp"

namespace ruis::layout {

// TODO: doxygen
class abstract_layout
{
protected:
	abstract_layout() = default;

public:
	abstract_layout(const abstract_layout&) = delete;
	abstract_layout& operator=(const abstract_layout&) = delete;

	abstract_layout(abstract_layout&&) = delete;
	abstract_layout& operator=(abstract_layout&&) = delete;

	virtual vector2 measure(
		const vector2& quotum, //
		const_widget_list& widgets
	) const = 0;

	/**
	 * @brief Arrange widgets.
	 * @param dims - dimensions of the area available to the layout.
	 * @param widgets - widgets to arrange.
	 */
	virtual void lay_out(
		const vector2& dims, //
		semiconst_widget_list& widgets
	) const = 0;

	virtual ~abstract_layout() = default;
};

extern const utki::shared_ref<abstract_layout> trivial;
extern const utki::shared_ref<abstract_layout> size;
extern const utki::shared_ref<abstract_layout> pile;
extern const utki::shared_ref<abstract_layout> row;
extern const utki::shared_ref<abstract_layout> column;

} // namespace ruis::layout
