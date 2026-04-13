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

#include "../../label/image.hpp"
#include "../../label/text.hpp"
#include "../choice_button.hpp"

namespace ruis::touch {

class tab_button :
	public ruis::choice_button, //
	private ruis::container
{
public:
	struct all_parameters {
		ruis::layout::parameters layout_params;
		ruis::widget::parameters widget_params;
		ruis::image::parameters image_params;
		ruis::text_widget::parameters text_params;
	};

	tab_button(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		ruis::string text
	);

	ruis::event_status on_mouse_button(const ruis::mouse_button_event& event) override;
};

namespace make {
utki::shared_ref<ruis::touch::tab_button> tab_button(
	utki::shared_ref<ruis::context> context, //
	ruis::touch::tab_button::all_parameters params,
	ruis::string text
);
} // namespace make

} // namespace ruis::touch
