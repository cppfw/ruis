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

#pragma once

#include "../../util/localization.hpp"

#include "text_widget.hpp"

namespace ruis {

class text_string_widget : public text_widget
{
	mutable ruis::rect bb{};

	string text_string;

protected:
	vector2 measure(const ruis::vector2& quotum) const noexcept override;

	text_string_widget(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);

	text_string_widget(
		utki::shared_ref<ruis::context> context,
		text_widget::parameters text_widget_params,
		string text
	);

	const ruis::rect& get_bounding_box() const
	{
		return this->bb;
	}

	void recompute_bounding_box();

	void set_text(string text);

public:
	using text_widget::set_text;

	void set_text(std::u32string text) override;

	std::u32string get_text() const override;

	void set_wording(wording w);

	/**
	 * @brief Get wording.
	 * @return Reference to the wording held by this instance.
	 * @throw std::ivalid_argument - in case this instance does not hold a wording.
	 */
	wording& get_wording();

	/**
	 * @brief Get actual text string.
	 * Obtain reference to the actual displayed string of text.
	 * @return Constant reference to the actual text string of the text_string_widget.
	 */
	const std::u32string& get_string() const noexcept;

	void on_font_change() override
	{
		this->recompute_bounding_box();
	}

	void on_text_change() override;

	void on_reload() override;
};

} // namespace ruis
