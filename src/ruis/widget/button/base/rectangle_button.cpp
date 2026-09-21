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

#include "rectangle_button.hpp"

using namespace ruis;

void rectangle_button::render(const mat4& matrix) const
{
	this->rectangle::render(matrix);
}

void rectangle_button::on_pressed_change()
{
	this->update_color();
	this->button::on_pressed_change();
}

void rectangle_button::on_enabled_change()
{
	this->widget::on_enabled_change();
	this->update_color();
}

void rectangle_button::update_color()
{
	if (!this->is_enabled()) {
		this->set_fill_color(this->params.disabled_color);
		this->set_stroke_color(this->params.disabled_stroke_color);
		this->set_stroke_width(this->params.disabled_stroke_width);
	} else if (this->is_pressed()) {
		this->set_fill_color(this->params.pressed_color);
		this->set_stroke_color(this->params.pressed_stroke_color);
		this->set_stroke_width(this->params.stroke_width);
	} else {
		this->set_fill_color(this->params.unpressed_color);
		this->set_stroke_color(this->params.unpressed_stroke_color);
		this->set_stroke_width(this->params.stroke_width);
	}
}

rectangle_button::rectangle_button( //
	const utki::shared_ref<ruis::context>& context,
	parameters params,
	widget_list contents //
) :
	widget(context, {}, {}),
	button(context, button::parameters{}),
	rectangle(
		context,
		// clang-format off
		{
			.params = [&](){
				for(auto& b : params.rectangle.padding.specific.borders){
					if(b.get().is_undefined()){
						b = context.get().style().get_len_gap();
					}
				}
				for(auto& r : params.rectangle.specific.corner_radii){
					if(r.get().is_undefined()){
						r = context.get().style().get_len_gap_small();
					}
				}
				return std::move(params.rectangle);
			}()
		},
		// clang-format on
		std::move(contents)
	),
	params([&]() {
		if (auto& c = params.specific.unpressed_color; c.get().is_undefined()) {
			c = context.get().style().get_color_primary();
		}
		if (auto& c = params.specific.pressed_color; c.get().is_undefined()) {
			c = context.get().style().get_color_secondary();
		}
		if (auto& c = params.specific.disabled_color; c.get().is_undefined()) {
			c = params.specific.pressed_color;
		}
		if (auto& c = params.specific.unpressed_stroke_color; c.get().is_undefined()) {
			c = context.get().style().get_color_primary();
		}
		if (auto& c = params.specific.pressed_stroke_color; c.get().is_undefined()) {
			c = context.get().style().get_color_secondary();
		}
		if (auto& c = params.specific.disabled_stroke_color; c.get().is_undefined()) {
			c = params.specific.unpressed_color;
		}

		if (auto& l = params.specific.disabled_stroke_width; l.get().is_undefined()) {
			l = context.get().style().get_len_border();
		}

		return std::move(params.specific);
	}())
{
	this->update_color();
}
