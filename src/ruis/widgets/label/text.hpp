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

#include "../../res/font.hpp"
#include "../base/single_line_text_widget.hpp"
#include "../widget.hpp"

namespace ruis {

/**
 * @brief Text label widget.
 * This widget shows text.
 * From GUI script it can be instantiated as "text".
 */
class text :
	public single_line_text_widget, //
	public color_widget
{
public:
	text(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);

	text(
		utki::shared_ref<ruis::context> context,
		widget::parameters widget_params,
		text_widget::parameters text_widget_params,
		color_widget::parameters color_widget_params,
		std::u32string text
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
[[deprecated]]
inline utki::shared_ref<ruis::widget> text(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params,
	std::u32string text,
	ruis::color_widget::parameters color_widget_params,
	ruis::text_widget::parameters text_widget_params
)
{
	return utki::make_shared<ruis::text>(
		std::move(context),
		std::move(widget_params),
		std::move(text_widget_params),
		std::move(color_widget_params),
		std::move(text)
	);
}

struct text_parameters {
	widget::parameters widget_params;
	color_widget::parameters color_params;
	text_widget::parameters text_params;
};

inline utki::shared_ref<ruis::widget> text(
	utki::shared_ref<ruis::context> context,
	text_parameters params,
	std::u32string text
)
{
	return utki::make_shared<ruis::text>(
		std::move(context),
		std::move(params.widget_params),
		std::move(params.text_params),
		std::move(params.color_params),
		std::move(text)
	);
}

} // namespace make

} // namespace ruis
