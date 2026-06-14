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

#include "trivial_layout.hpp"

namespace ruis::layout {

/**
  * @brief Size layout.
  * This layout only modifies dimensions of widgets according to their layout parameters,
  * it does not change position of widgets at all.
 */
// TODO: is this layout used anywhere??? If not, then remove.
class size_layout : public trivial_layout
{
public:
	void lay_out(
		const vec2& dims, //
		semiconst_widget_list& widgets
	) const override;
};

} // namespace ruis::layout
