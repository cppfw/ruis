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

void text_widget::set_font_face(const utki::shared_ref<const res::font>& font_res)
{
	if (this->params.font_face == font_res.to_shared_ptr()) {
		return;
	}

	this->params.font_face = font_res.to_shared_ptr();

	this->update_fonts();
}

void text_widget::set_font_size(styled<length> size)
{
	if (this->params.font_size == size) {
		return;
	}

	this->params.font_size = std::move(size);

	this->update_fonts();
}

void text_widget::update_fonts()
{
	for (size_t i = 0; i != size_t(res::font::style::enum_size); ++i) {
		// TODO: allow leaving it null
		// always set in constructor to at least default font
		ASSERT(this->params.font_face.get())

		this->fonts[i] = this->params.font_face.get()->get(
			this->params.font_size.get().get(this->context), //
			res::font::style(i)
		);
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
		if (!p.font_face.get()) {
			// TODO: allow null font face
			p.font_face = this->context.get().loader().load<res::font>("ruis_fnt_text");
		}
		// TODO: allow size to be undefined
		if (p.font_size.get().is_undefined()) {
			p.font_size = this->context.get().units.pp_to_px(parameters::default_font_size_pp);
		}
		return p;
	}()),
	fonts{
		this->params.font_face.get()->get(
			this->params.font_size.get().get(this->context), //
			res::font::style::normal
		),
		this->params.font_face.get()->get(
			this->params.font_size.get().get(this->context), //
			res::font::style::bold
		),
		this->params.font_face.get()->get(
			this->params.font_size.get().get(this->context), //
			res::font::style::italic
		),
		this->params.font_face.get()->get(
			this->params.font_size.get().get(this->context), //
			res::font::style::bold_italic
		)
	}
{}
