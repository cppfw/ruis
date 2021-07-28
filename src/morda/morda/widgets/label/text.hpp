/*
morda GUI framework

Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

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

#include "../widget.hpp"
#include "../../res/font.hpp"
#include "../base/text_widget.hpp"

namespace morda{

/**
 * @brief Text label widget.
 * This widget shows text.
 * From GUI script it can be instantiated as "text".
 */
class text :
		public single_line_text_widget,
		public color_widget
{
public:
	text(std::shared_ptr<morda::context> c, const treeml::forest& desc);

public:
	~text()noexcept{}

	void render(const morda::matrix4& matrix)const override;
};

}
