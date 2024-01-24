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

#include "text_widget.hpp"

#include "../../context.hpp"
#include "../../util/util.hpp"

using namespace ruis;

namespace {

constexpr auto default_font_size_pp = 12;

} // namespace

void text_widget::set_font_face(const utki::shared_ref<const res::font>& font_res)
{
	this->font_face = font_res;

	this->set_font_size(this->font_size);
}

void text_widget::set_font_size(real size)
{
	// do not exit early if requested size is same as the current one,
	// because set_font_size() is called from within set_font_face() to
	// create fonts for all font styles

	this->font_size = size;

	for (size_t i = 0; i != size_t(res::font::style::enum_size); ++i) {
		this->fonts[i] = this->font_face.get().get(this->font_size, res::font::style(i));
	}

	this->invalidate_layout();

	this->on_font_change();
}

text_widget::text_widget(const utki::shared_ref<ruis::context>& c, const treeml::forest& desc) :
	widget(c, desc),
	font_size([&desc, this]() {
		for (const auto& p : desc) {
			if (!is_property(p)) {
				continue;
			}

			if (p.value == "font_size") {
				return parse_dimension_value(get_property_value(p), this->context.get().units);
			}
		}

		return this->context.get().units.pp_to_px(default_font_size_pp);
	}()),
	font_face([&desc, this]() {
		for (const auto& p : desc) {
			if (!is_property(p)) {
				continue;
			}

			if (p.value == "font") {
				return this->context.get().loader.load<ruis::res::font>(get_property_value(p).to_string().c_str());
			}
		}

		// load default font
		return this->context.get().loader.load<res::font>("ruis_fnt_text");
	}()),
	fonts{
		this->font_face.get().get(this->font_size, res::font::style(0)),
		this->font_face.get().get(this->font_size, res::font::style(1)),
		this->font_face.get().get(this->font_size, res::font::style(2)),
		this->font_face.get().get(this->font_size, res::font::style(3))
	}
{
	for (const auto& p : desc) {
		if (!is_property(p)) {
			continue;
		}
	}
}
