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

#include "../base/nine_patch_button.hpp"
#include "../toggle_button.hpp"

namespace ruis {

class nine_patch_toggle :
	virtual public toggle_button, //
	public nine_patch_button
{
protected:
	nine_patch_toggle(//
		utki::shared_ref<ruis::context> context, //
		container::parameters container_params,
		button::parameters button_params,
		blending_widget::parameters blending_params,
		nine_patch::parameters nine_patch_params,
		nine_patch_button::parameters nine_patch_button_params,
		widget_list children
	);

protected:
	void on_pressed_change() override;

	bool on_mouse_button(const mouse_button_event& e) override
	{
		if (this->nine_patch_button::on_mouse_button(e)) {
			return true;
		}
		return this->toggle_button::on_mouse_button(e);
	}
};

} // namespace ruis
