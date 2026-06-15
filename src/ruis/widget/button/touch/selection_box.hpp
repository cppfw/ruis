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

#include "../impl/rectangle_push_button.hpp"
#include "../selection_box.hpp"

// #include "../../label/rectangle.hpp"

namespace ruis::touch {

class selection_box :
	public virtual ruis::widget, //
	public rectangle_push_button,
	public ruis::selection_box
{
public:
	struct all_parameters{
		// parameters for ruis::selection_box
		layout::parameters layout_params;
		widget::parameters widget_params;
		list_widget::parameters list_params;
		string title;

		// parameters specific to ruis::touch::selection_box
		// TODO: add rectangle_push_button params
	};

	selection_box(
		utki::shared_ref<ruis::context> context, //
		all_parameters params
	);

	void on_click() override;

private:
	void show_selection_menu();
};

namespace make {
utki::shared_ref<ruis::touch::selection_box> selection_box(
	utki::shared_ref<ruis::context> context, //
	ruis::touch::selection_box::all_parameters params
);
} // namespace make

} // namespace ruis::touch
