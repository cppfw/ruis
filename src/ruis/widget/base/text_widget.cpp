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

#include "text_widget.hpp"

#include "../../context.hpp"
#include "../../util/util.hpp"

using namespace ruis;

namespace {

constexpr auto default_font_size_pp = 12;

} // namespace

void text_widget::set_font_face(const utki::shared_ref<const res::font>& font_res)
{
	this->params.font_face = font_res.to_shared_ptr();

	this->set_font_size(this->params.font_size);
}

void text_widget::set_font_size(length size)
{
	// do not exit early if requested size is same as the current one,
	// because set_font_size() is called from within set_font_face() to
	// create fonts for all font styles

	this->params.font_size = size;

	for (size_t i = 0; i != size_t(res::font::style::enum_size); ++i) {
		ASSERT(this->params.font_face) // always set in constructor to at least default font
		this->fonts[i] = this->params.font_face->get(this->params.font_size.get(this->context), res::font::style(i));
	}

	this->invalidate_layout();

	this->on_font_change();
}

text_widget::text_widget(
	utki::shared_ref<ruis::context> context, //
	parameters params
) :
	widget(std::move(context), {}, {}),
	params([this, &params]() {
		auto p = std::move(params);
		if (!p.font_face) {
			p.font_face = this->context.get().loader.load<res::font>("ruis_fnt_text");
		}
		if (p.font_size.is_undefined()) {
			p.font_size = this->context.get().units.pp_to_px(default_font_size_pp);
		}
		return p;
	}()),
	fonts{
		this->params.font_face->get(this->params.font_size.get(this->context), res::font::style::normal),
		this->params.font_face->get(this->params.font_size.get(this->context), res::font::style::bold),
		this->params.font_face->get(this->params.font_size.get(this->context), res::font::style::italic),
		this->params.font_face->get(this->params.font_size.get(this->context), res::font::style::bold_italic)
	}
{}
