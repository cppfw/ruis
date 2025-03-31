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

#pragma once

#include <list>
#include <vector>

#include <r4/rectangle.hpp>
#include <utki/enum_array.hpp>

#include "../../res/font.hpp"
#include "../widget.hpp"

#include "color_widget.hpp"

namespace ruis {

/**
 * @brief Abstract widget displaying a text.
 */
class text_widget : virtual public widget
{
public:
	struct parameters {
		constexpr static const auto default_font_size_pp = 12;

		styled<length> font_size = length::make_pp(default_font_size_pp);
		styled<res::font> font_face;

		constexpr static const auto default_selection_color = 0xff804040;
		styled<ruis::color> selection_color = default_selection_color;
	};

private:
	parameters params;

	utki::enum_array<std::shared_ptr<const ruis::font>, res::font::style> fonts;

	void update_fonts();
	void update_fonts_and_notify();

protected:
	/**
	 * @brief Constructor.
	 * If font face in parameters is not set, then tries to set it to 'font_face_normal'
	 * from current default style of the ruis context.
	 * If font size in parameters is undefined, then tries to set it to 'font_size_normal'
	 * from current default style of the ruis context.
	 * @param context - ruis context.
	 * @param params - text widget parameters.
	 */
	text_widget(
		utki::shared_ref<ruis::context> context, //
		parameters params
	);

	const parameters& get_params() const noexcept
	{
		return this->params;
	}

public:
	text_widget(const text_widget&) = delete;
	text_widget& operator=(const text_widget&) = delete;

	text_widget(text_widget&&) = delete;
	text_widget& operator=(text_widget&&) = delete;

	~text_widget() override = default;

	void set_font_face(styled<res::font> font_face);

	void set_font_size(styled<length> size);

	const length& get_font_size() const noexcept
	{
		return this->params.font_size.get();
	}

	/**
	 * @brief Get font for drawing.
	 * @return Font for drawing.
	 * @throw std::logic_error in case there is no font face currently set.
	 */
	const ruis::font& get_font(res::font::style style = res::font::style::normal) const;

	void set_text(std::string_view text)
	{
		this->set_text(utki::to_utf32(text));
	}

	virtual void set_text(std::u32string text) = 0;

	virtual std::u32string get_text() const = 0;

	void clear()
	{
		this->set_text(std::u32string());
	}

	virtual void on_font_change() {}

	virtual void on_text_change()
	{
		if (this->text_change_handler) {
			this->text_change_handler(*this);
		}
	}

	std::function<void(text_widget& w)> text_change_handler;

private:
};

} // namespace ruis
