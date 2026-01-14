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

#include "text_widget.hpp"

#include "../../context.hpp"
#include "../../util/util.hpp"

using namespace ruis;

void text_widget::set_font_face(styled<res::font> font_face)
{
	if (this->params.font_face == font_face) {
		return;
	}

	this->params.font_face = std::move(font_face);

	this->update_fonts_and_notify();
}

void text_widget::set_font_size(styled<length> size)
{
	if (this->params.font_size == size) {
		return;
	}

	this->params.font_size = std::move(size);

	this->update_fonts_and_notify();
}

const ruis::font& text_widget::get_font(res::font::style style) const
{
	const auto& f = this->fonts[style];
	if (!f) {
		throw std::logic_error("text_widget::get_font(): font face is not set");
	}

	return *f;
}

void text_widget::update_fonts()
{
	if (!this->params.font_face.get()) {
		// font face is not set
		for (auto& f : this->fonts) {
			f.reset();
		}
		return;
	}

	real font_size = [&]() -> real {
		if (this->params.font_size.get().is_undefined()) {
			return length::make_pp(parameters::default_font_size_pp).get(this->context);
		}
		return this->params.font_size.get().get(this->context);
	}();

	for (auto [v, e] : this->fonts.zip_with_enum()) {
		v = this->params.font_face.get()->get(
			font_size, //
			e
		);
	}
}

void text_widget::update_fonts_and_notify()
{
	this->update_fonts();
	this->invalidate_layout();
	this->on_font_change();
}

text_widget::text_widget(
	utki::shared_ref<ruis::context> context, //
	parameters params
) :
	widget(std::move(context), {}, {}),
	params([&]() {
		if (!params.font_face.get() && !params.font_face.is_from_style()) {
			params.font_face = this->context.get().style().get_font_face_normal();
		}
		if (params.font_size.get().is_undefined() && !params.font_size.is_from_style()) {
			params.font_size = this->context.get().style().get_font_size_normal();
		}
		return std::move(params);
	}())
{
	this->update_fonts();
}
