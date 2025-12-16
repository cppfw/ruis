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

#include "../base/image_button.hpp"
#include "../push_button.hpp"

namespace ruis {
class image_push_button :
	virtual public push_button, //
	public image_button
{
protected:
	void on_pressed_change() override;

public:
	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
		button::parameters button_params;
		blending_widget::parameters blending_params;
		image::parameters image_params;
		image_button::parameters image_button_params;
	};

	image_push_button( //
		utki::shared_ref<ruis::context> context,
		all_parameters params
	);
};

namespace make {
utki::shared_ref<ruis::image_push_button> image_push_button(
	utki::shared_ref<ruis::context> context, //
	ruis::image_push_button::all_parameters params
);
} // namespace make
} // namespace ruis
