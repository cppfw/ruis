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

#include "../../label/image.hpp"
#include "../../label/nine_patch.hpp"
#include "../toggle_button.hpp"

namespace ruis {

/**
 * @brief Checkbox widget.
 * Checkbox is a toggle button which shows tick sign when checked and empty box when unchecked.
 * In GUI script can be instantiated as 'Checkbox'. Only available after initializing
 * standard ruis widgets.
 */
// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "std::shared_from_this is public via toggle_button")
class check_box :
	virtual public toggle_button, //
	private nine_patch
{
	// TODO: refactor to use utki::shared_ref
	// std::shared_ptr<widget> check_widget;
	image& tick_widget;

public:
	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		button::parameters button_params;
		nine_patch::parameters nine_patch_params;
		image::parameters image_params;
	};

	check_box(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	);

	check_box(const check_box&) = delete;
	check_box& operator=(const check_box&) = delete;

	check_box(check_box&&) = delete;
	check_box& operator=(check_box&&) = delete;

	~check_box() override = default;

	bool on_mouse_button(const mouse_button_event& e) override
	{
		return this->toggle_button::on_mouse_button(e);
	}

	void on_hovered_change(unsigned pointer_id) override
	{
		this->toggle_button::on_hovered_change(pointer_id);
	}

	void on_pressed_change() override;

private:
};

namespace make {
inline utki::shared_ref<ruis::check_box> check_box( //
	utki::shared_ref<ruis::context> context,
	check_box::all_parameters params
)
{
	return utki::make_shared<ruis::check_box>( //
		std::move(context),
		std::move(params)
	);
}
} // namespace make

} // namespace ruis
