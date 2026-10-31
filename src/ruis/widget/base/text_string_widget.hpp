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

#pragma once

#include <optional>
#include <string_view>

#include "../../util/localization.hpp"

#include "color_widget.hpp"
#include "font_widget.hpp"

namespace ruis {

/**
 * @brief Base widget that holds a text string and renders it with a font.
 * This is the common base for the text label (ruis::text) and the text input field
 * (ruis::text_input). It only knows about holding a string and about its font/color; it does
 * not know anything about how the text is laid out (e.g. broken into lines), because the
 * layout logic differs between the subclasses.
 */
class text_string_widget :
	virtual public widget, //
	public color_widget,
	public font_widget
{
	string text_string;

public:
	struct specific_parameters {
		constexpr static const auto default_selection_color = 0xff804040;
		styled<ruis::color> selection_color = default_selection_color;

		/**
		 * @brief Font style (normal, bold, italic, bold_italic).
		 * If undefined, the default style is normal.
		 */
		std::optional<res::font::style> style;
	};

	struct parameters {
		styled<ruis::color> color;
		font_widget::parameters font;
		specific_parameters specific;
	};

private:
	specific_parameters params;

protected:
	const specific_parameters& get_params() const noexcept
	{
		return this->params;
	}

	vec2 measure(
		const ruis::vec2& quotum, //
		const r4::vector2<measure_mode>& mode
	) const noexcept override;

	text_string_widget(
		const utki::shared_ref<ruis::context>& context, //
		parameters params,
		string text
	);

	/**
	 * @brief Get font for text drawing.
	 * Uses the widget's font style, or normal if style is not set.
	 * @return Font for drawing.
	 * @throw std::logic_error in case there is no font face currently set.
	 */
	const ruis::font& get_font() const
	{
		utki::assert(this->params.style.has_value());
		return font_widget::get_font(
			// Use value_or() to silence linter.
			this->params.style.value_or(res::font::style::normal)
		);
	}

	/**
	 * @brief Set text without triggering the on_text_change() hook nor the text_change_handler.
	 * Intended for internal use by subclasses that update the text as part of an edit
	 * and manage cursor/selection state themselves.
	 */
	void set_string_no_notify(string text);

public:
	/**
	 * @brief Set text.
	 * @param text - string to set.
	 */
	void set_string(string text);

	void set_string(std::string text);

	const string& get_string() const
	{
		return this->text_string;
	}

	void clear()
	{
		this->set_string(std::u32string());
	}

	virtual void on_text_change();

	void on_reload() override;

	std::function<void(text_string_widget& w)> text_change_handler;
};

} // namespace ruis
