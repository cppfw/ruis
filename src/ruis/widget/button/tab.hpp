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

#include "../label/nine_patch.hpp"

#include "impl/nine_patch_toggle.hpp"

#include "choice_button.hpp"

namespace ruis {

class tab :
	virtual public choice_button, //
	public nine_patch_toggle
{
	bool mask_overlaps(vector2 pos);

public:
	struct all_parameters {
		ruis::layout::parameters layout_params;
		ruis::widget::parameters widget_params;
		container::parameters container_params;
		button::parameters button_params;
		blending_widget::parameters blending_params;
		nine_patch::parameters nine_patch_params;
		nine_patch_button::parameters nine_patch_button_params;
	};

	tab( //
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		widget_list children
	);

	tab(const tab&) = delete;
	tab& operator=(const tab&) = delete;

	tab(tab&&) = delete;
	tab& operator=(tab&&) = delete;

	~tab() override = default;

protected:
	bool on_mouse_button(const mouse_button_event& event) override;

	void on_hovered_change(unsigned pointer_id) override {}

	void on_pressed_change() override;
};

namespace make {
inline utki::shared_ref<ruis::tab> tab(
	utki::shared_ref<ruis::context> context, //
	ruis::tab::all_parameters params,
	widget_list children
)
{
	return utki::make_shared<ruis::tab>(
		std::move(context), //
		std::move(params),
		std::move(children)
	);
}

} // namespace make
} // namespace ruis
