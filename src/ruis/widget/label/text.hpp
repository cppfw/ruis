/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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
#include "../base/text_string_widget.hpp"
#include "../widget.hpp"

namespace ruis {

/**
 * @brief Text label widget.
 * This widget shows text.
 * From GUI script it can be instantiated as "text".
 */
class text :
	public text_string_widget, //
	public color_widget
{
public:
	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		color_widget::parameters color_params;
		text_widget::parameters text_params;
	};

	text(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		string text
	);

public:
	text(const text&) = delete;
	text& operator=(const text&) = delete;

	text(text&&) = delete;
	text& operator=(text&&) = delete;

	~text() override = default;

	void render(const ruis::matrix4& matrix) const override;
};

namespace make {
inline utki::shared_ref<ruis::text> text(
	utki::shared_ref<ruis::context> context, //
	text::all_parameters params,
	string text = {}
)
{
	return utki::make_shared<ruis::text>(
		std::move(context), //
		std::move(params),
		std::move(text)
	);
}
} // namespace make

} // namespace ruis
