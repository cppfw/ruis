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

#include "../base/labeled_widget.hpp"

#include "text_field.hpp"

namespace ruis {

class labeled_text_field :
	public labeled_widget, //
	public text_field
{
public:
	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
		color_widget::parameters color_params;
		text_widget::parameters text_widget_params;
		bare_text_field::parameters text_field_params;
		labeled_widget::parameters label_params;
	};

	labeled_text_field(
		const utki::shared_ref<ruis::context> context, //
		bare_text_field& input_field,
		text& label
	);
};

} // namespace ruis
