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
#include "../push_button.hpp"

namespace ruis {

class nine_patch_push_button :
	public push_button, //
	public nine_patch_button
{
public:
	struct all_parameters {
		layout_parameters layout_params;
		ruis::widget::parameters widget_params;
		container::parameters container_params;
		button::parameters button_params;
		blending_widget::parameters blending_params;
		nine_patch::parameters nine_patch_params;
		nine_patch_button::parameters nine_patch_button_params;
	};

	nine_patch_push_button(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		widget_list children
	);

protected:
	void on_pressed_change() override;

	bool on_mouse_button(const mouse_button_event& e) override
	{
		return this->push_button::on_mouse_button(e);
	}

	void on_hovered_change(unsigned pointer_id) override
	{
		this->push_button::on_hovered_change(pointer_id);
	}
};

namespace make {
inline utki::shared_ref<ruis::nine_patch_push_button> nine_patch_push_button(
	utki::shared_ref<ruis::context> context, //
	nine_patch_push_button::all_parameters params,
	widget_list children
)
{
	return utki::make_shared<ruis::nine_patch_push_button>(
		std::move(context), //
		std::move(params),
		std::move(children)
	);
}

} // namespace make

} // namespace ruis
