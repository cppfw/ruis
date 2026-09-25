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

#include "../../util/localization.hpp"

#include "color_widget.hpp"
#include "font_widget.hpp"

namespace ruis {

// TODO: doxygen
class text_string_widget :
	virtual public widget, //
	public color_widget,
	public font_widget
{
	mutable ruis::rect bb{};

	string text_string;

public:
	struct specific_parameters {
		constexpr static const auto default_selection_color = 0xff804040;
		styled<ruis::color> selection_color = default_selection_color;
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

	vec2 measure(const ruis::vec2& quotum) const noexcept override;

	text_string_widget(
		const utki::shared_ref<ruis::context>& context, //
		parameters params,
		string text
	);

	const ruis::rect& get_bounding_box() const
	{
		return this->bb;
	}

	void recompute_bounding_box();

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

	void on_font_change() override
	{
		this->recompute_bounding_box();
	}

	virtual void on_text_change();

	void on_reload() override;

	std::function<void(text_string_widget& w)> text_change_handler;

private:
	void notify_text_change()
	{
		if (this->text_change_handler) {
			this->text_change_handler(*this);
		}
	}
};

} // namespace ruis
