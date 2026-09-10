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

#include "../../label/nine_patch.hpp"
#include "../text_field.hpp"

namespace ruis {

/**
 * @brief Text field widget with a nine-patch background.
 * This widget combines a nine_patch and a text_input as its child.
 */
class nine_patch_text_field :
	virtual public widget, //
	private nine_patch,
	public text_field
{
public:
	struct all_parameters {
		layout::parameters layout_params;
		widget::parameters widget_params;
		container::parameters container_params;
		padding::parameters padding_params;
		nine_patch::parameters nine_patch_params;
		text_input::parameters text_input_params;
	};

private:
	nine_patch_text_field(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters& params,
		utki::shared_ref<ruis::text_input> text_input
	);

public:
	nine_patch_text_field(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters params,
		ruis::string text
	);
};

namespace make {
utki::shared_ref<ruis::nine_patch_text_field> nine_patch_text_field(
	const utki::shared_ref<ruis::context>& context, //
	ruis::nine_patch_text_field::all_parameters params,
	ruis::string text
);
} // namespace make

} // namespace ruis
