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
	utki::shared_ref<ruis::context> context,
	text_widget::parameters text_widget_params,
	string text
) :
	widget(
		std::move(context), //
		{},
		{}
	),
	text_widget(
		this->context, //
		std::move(text_widget_params)
	),
	text_string(std::move(text))
{
	this->recompute_bounding_box();
}

void text_string_widget::recompute_bounding_box()
{
	this->bb = this->get_font().get_bounding_box(this->get_string());
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
	this->text_widget::on_text_change();
}

void text_string_widget::set_text(string text)
{
	this->text_string = std::move(text);
	this->invalidate_layout();
	this->on_text_change();
}

void text_string_widget::set_text(std::u32string text)
{
	this->set_text(string(text));
}

const std::u32string& text_string_widget::get_string() const noexcept
{
	if (std::holds_alternative<std::u32string>(this->text_string)) {
		return *std::get_if<std::u32string>(&this->text_string);
	}
	ASSERT(std::holds_alternative<wording>(this->text_string));
	return std::get_if<wording>(&this->text_string)->string();
}

std::u32string text_string_widget::get_text() const
{
	return this->get_string();
}

wording& text_string_widget::get_wording()
{
	if (!std::holds_alternative<wording>(this->text_string)) {
		throw std::invalid_argument("text_string_widget(): this instance does not hold a wording");
	}

	return *std::get_if<wording>(&this->text_string);
}

void text_string_widget::set_wording(wording w)
{
	this->set_text(std::move(w));
}

void text_string_widget::on_reload()
{
	if (std::holds_alternative<wording>(this->text_string)) {
		auto& w = this->get_wording();
		auto new_wording = this->context.get().localization.get().reload(std::move(w));
		this->set_wording(std::move(new_wording));
	}
}
