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

#include "color_widget.hpp"

#include "../../util/util.hpp"

using namespace ruis;

color_widget::color_widget(
	const utki::shared_ref<ruis::context>& context,
	styled<ruis::color> color //
) :
	widget(context, {}, {}),
	color(std::move(color))
{}

void color_widget::set_color(styled<ruis::color> color)
{
	if (this->color == color) {
		return;
	}

	this->color = std::move(color);
	this->clear_cache();
	this->on_color_change();
}
