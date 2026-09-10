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

#include <list>
#include <vector>

#include "../widget.hpp"

#include "color_widget.hpp"
#include "font_widget.hpp"

namespace ruis {

/**
 * @brief Abstract widget displaying a text.
 */
class text_widget :
	virtual public widget, //
	public color_widget,
	public font_widget
{
public:
	struct parameters {
		color_widget::parameters color_params;
		font_widget::parameters font_params;

		constexpr static const auto default_selection_color = 0xff804040;
		styled<ruis::color> selection_color = default_selection_color;
	};

private:
	parameters params; // TODO: save only selection color

protected:
	/**
	 * @brief Constructor.
	 * If font face in parameters is not set, then tries to set it to 'font_face_normal'
	 * from current default style of the ruis context.
	 * If font size in parameters is undefined, then tries to set it to 'font_size_normal'
	 * from current default style of the ruis context.
	 * @param context - ruis context.
	 * @param color_params - text color parameters.
	 * @param params - text widget parameters.
	 */
	text_widget(
		const utki::shared_ref<ruis::context>& context, //
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

	void set_text(std::string_view text)
	{
		this->set_text(utki::to_utf32(text));
	}

	virtual void set_text(std::u32string text) = 0;

	/**
	 * @brief Get text as UTF-32 string.
	 * The internal representation of the text in particular implemntation of the text_widget
	 * may be different from UTF-32, so this function returns a new UTF-32 string object each time it is called.
	 * @return Text as UTF-32 string.
	 */
	virtual std::u32string get_text() const = 0;

	void clear()
	{
		this->set_text(std::u32string());
	}

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
