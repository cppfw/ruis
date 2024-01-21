/*
morda - GUI framework

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

#include "../../res/font.hpp"
#include "../../widget.hpp"
#include "../base/single_line_text_widget.hpp"

namespace morda {

/**
 * @brief Text label widget.
 * This widget shows text.
 * From GUI script it can be instantiated as "text".
 */
class text : public single_line_text_widget, public color_widget
{
public:
	text(const utki::shared_ref<morda::context>& c, const treeml::forest& desc);

public:
	text(const text&) = delete;
	text& operator=(const text&) = delete;

	text(text&&) = delete;
	text& operator=(text&&) = delete;

	~text() override = default;

	void render(const morda::matrix4& matrix) const override;
};

} // namespace morda
