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

#include "ellipse_button.hpp"

using namespace ruis;

void ellipse_button::render(const mat4& matrix) const
{
	this->ellipse::render(matrix);
}

void ellipse_button::on_pressed_change()
{
	this->update_color();
	this->button::on_pressed_change();
}

void ellipse_button::update_color()
{
	if (this->is_pressed()) {
		this->set_color(this->params.pressed_color);
	} else {
		this->set_color(this->params.unpressed_color);
	}
}

ellipse_button::ellipse_button(
	const utki::shared_ref<ruis::context>& context, //
	parameters params,
	widget_list contents
) :
	widget(context, {}, {}),
	button(context, {}),
	ellipse(
		context,
		// clang-format off
		{
			.params = [&](){
				for(auto& b : params.ellipse.padding.specific.borders){
					if(b.get().is_undefined()){
						b = context.get().style().get_len_gap();
					}
				}

				return std::move(params.ellipse);
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

		return std::move(params.specific);
	}())
{
	this->update_color();
}