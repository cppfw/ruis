/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#include "../../container.hpp"

#include "choice_button.hpp"

namespace morda {

/**
 * @brief Radio button widget.
 * Radio button is a choice button which displays a thick dot when checked and
 * an empty round when unchecked. In GUI script it can be instantiated as
 * "radio_button". Only available after initializing standard morda widgets.
 */
class radio_button : virtual public choice_button, private container
{
	const utki::shared_ref<widget> check_widget;

public:
	radio_button(const utki::shared_ref<morda::context>& c, const treeml::forest& desc);

	radio_button(const radio_button&) = delete;
	radio_button& operator=(const radio_button&) = delete;

	bool on_mouse_button(const mouse_button_event& e) override
	{
		return this->choice_button::on_mouse_button(e);
	}

	void on_hover_change(unsigned pointer_id) override
	{
		this->choice_button::on_hover_change(pointer_id);
	}

	void on_press_change() override;

private:
};

} // namespace morda
