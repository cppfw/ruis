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

#include "text_widget.hpp"

#include "../../context.hpp"
#include "../../util/util.hpp"

using namespace morda;

void text_widget::set_font(const utki::shared_ref<const res::font>& font)
{
	this->font = font;

	this->invalidate_layout();

	this->on_font_change();
}

text_widget::text_widget(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
	widget(c, desc),
	font([&desc, this]() {
		for (const auto& p : desc) {
			if (!is_property(p)) {
				continue;
			}

			if (p.value == "font") {
				return this->context.get().loader.load<morda::res::font>(get_property_value(p).to_string().c_str());
			}
		}

		// load default font
		return this->context.get().loader.load<res::font>("morda_fnt_text");
	}())
{
	for (const auto& p : desc) {
		if (!is_property(p)) {
			continue;
		}
	}
}
