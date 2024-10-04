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

#include "text_string_widget.hpp"

using namespace ruis;

text_string_widget::text_string_widget(
	utki::shared_ref<ruis::context> context,
	text_widget::parameters text_widget_params,
	std::u32string text
) :
	widget(std::move(context), {}, {}),
	text_widget(this->context, std::move(text_widget_params)),
	text_string(std::move(text))
{
	this->recompute_bounding_box();
}

text_string_widget::text_string_widget(
	utki::shared_ref<ruis::context> context,
	text_widget::parameters text_widget_params,
	wording localized_text
) :
	widget(std::move(context), {}, {}),
	text_widget(this->context, std::move(text_widget_params)),
	text_string(std::move(localized_text))
{
	this->recompute_bounding_box();
}

text_string_widget::text_string_widget(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
	widget(c, desc),
	text_widget(this->context, desc)
{
	for (const auto& p : desc) {
		if (!is_property(p)) {
			continue;
		}

		if (p.value == "text") {
			this->text_string = utki::to_utf32(get_property_value(p).string);
			this->recompute_bounding_box();
		}
	}
}

void text_string_widget::recompute_bounding_box()
{
	this->bb = this->get_font().get_bounding_box(this->get_text_string());
}

vector2 text_string_widget::measure(const ruis::vector2& quotum) const noexcept
{
	vector2 ret(this->bb.d.x(), this->get_font().get_height());

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
	this->text_widget::on_text_change();
}

void text_string_widget::set_text(std::u32string text)
{
	this->text_string = std::move(text);
	this->invalidate_layout();
	this->on_text_change();
}

const std::u32string& text_string_widget::get_text_string() const noexcept
{
	if (std::holds_alternative<std::u32string>(this->text_string)) {
		return std::get<std::u32string>(this->text_string);
	}
	ASSERT(std::holds_alternative<wording>(this->text_string));
	return std::get<wording>(this->text_string).string();
}

std::u32string text_string_widget::get_text() const
{
	return this->get_text_string();
}
