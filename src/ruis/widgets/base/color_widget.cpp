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

#include "color_widget.hpp"

#include "../../util/util.hpp"

using namespace ruis;

color_widget::color_widget(utki::shared_ref<ruis::context> context, parameters params) :
	widget(std::move(context), widget::parameters{}),
	params(std::move(params))
{}

color_widget::color_widget(const utki::shared_ref<ruis::context>& c, const treeml::forest& desc) :
	widget(c, desc)
{
	for (const auto& p : desc) {
		if (!is_property(p)) {
			continue;
		}

		if (p.value == "color") {
			this->params.color = get_property_value(p).to_uint32();
		} else if (p.value == "disabled_color") {
			this->params.disabled_color = get_property_value(p).to_uint32();
		}
	}
}

void color_widget::set_color(uint32_t color)
{
	if (this->params.color == color) {
		return;
	}

	this->params.color = color;
	this->clear_cache();
}

void color_widget::set_disabled_color(uint32_t color)
{
	if (this->params.disabled_color == color) {
		return;
	}

	this->params.disabled_color = color;
	this->clear_cache();
}

uint32_t color_widget::get_current_color() const noexcept
{
	if (this->is_enabled()) {
		return this->get_color();
	} else {
		return this->get_disabled_color();
	}
}
