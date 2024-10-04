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

#include "text_widget.hpp"

namespace ruis {

class text_string_widget : public text_widget
{
	mutable ruis::rect bb{};

	std::u32string text;

protected:
	vector2 measure(const ruis::vector2& quotum) const noexcept override;

	text_string_widget(const utki::shared_ref<ruis::context>& c, const tml::forest& desc);

	text_string_widget(
		utki::shared_ref<ruis::context> context,
		text_widget::parameters text_widget_params,
		std::u32string text
	);

	const ruis::rect& get_bounding_box() const
	{
		return this->bb;
	}

	void recompute_bounding_box();

public:
	using text_widget::set_text;

	void set_text(std::u32string text) override;

	std::u32string get_text() const override;

	void on_font_change() override
	{
		this->recompute_bounding_box();
	}

	void on_text_change() override;
};

} // namespace ruis
