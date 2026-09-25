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

#include "text_string_widget.hpp"

using namespace ruis;

text_string_widget::text_string_widget(
	const utki::shared_ref<ruis::context>& context,
	parameters params,
	string text
) :
	widget(
		context, //
		{},
		{}
	),
	color_widget(
		context, //
		[&]() {
			if (params.color.get().is_undefined()) {
				params.color = context.get().style().get_color_text();
			}
			return std::move(params.color);
		}()
	),
	font_widget(
		context, //
		std::move(params.font)
	),
	text_string(std::move(text)),
	params(std::move(params.specific))
{
	this->recompute_bounding_box();
}

void text_string_widget::recompute_bounding_box()
{
	this->bb = this->get_font().get_bounding_box(this->get_string().get());
}

vec2 text_string_widget::measure(const ruis::vec2& quotum) const noexcept
{
	vec2 ret(this->bb.d.x(), this->get_font().get_height());

	for (unsigned i = 0; i != ret.size(); ++i) {
		if (quotum[i] >= 0) {
			ret[i] = quotum[i];
		}
	}

	return ret;
}

void text_string_widget::on_text_change()
{
	this->recompute_bounding_box();
	this->notify_text_change();
}

void text_string_widget::set_string(string text)
{
	this->text_string = std::move(text);
	this->invalidate_layout(); // TODO: do not invalidate in text_input?
	this->on_text_change();
}

void text_string_widget::set_string(std::string text)
{
	this->set_string(utki::to_utf32(text));
}

void text_string_widget::on_reload()
{
	if (this->text_string.is_wording()) {
		auto& w = this->text_string.get_wording();
		auto new_wording = this->context.get().localization.get().reload(std::move(w));
		this->set_string(std::move(new_wording));
	}

	this->widget::on_reload();
}
