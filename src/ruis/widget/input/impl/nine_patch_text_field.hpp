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

#include <memory>

#include "../../button/impl/image_push_button.hpp"
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
	struct parameters {
		ruis::nine_patch::parameters nine_patch;
		ruis::text_input::parameters text_input;
		ruis::text_field::parameters text_field;
	};

	struct all_parameters {
		ruis::layout_parameters layout_params;
		ruis::widget::parameters widget;
		parameters params;
	};

private:
	// The clear button (to the right of the text_input). Null if clear_button is disabled.
	std::shared_ptr<ruis::image_push_button> clear_button;

	// Returns nullptr when 'enabled' is false.
	static std::shared_ptr<ruis::image_push_button> make_clear_button(
		const utki::shared_ref<ruis::context>& context, //
		bool enabled
	);

	static widget_list make_content_children(
		const utki::shared_ref<ruis::text_input>& text_input, //
		const std::shared_ptr<ruis::image_push_button>& clear_button
	);

	nine_patch_text_field(
		const utki::shared_ref<ruis::context>& context, //
		all_parameters& params,
		utki::shared_ref<ruis::text_input> text_input, //
		std::shared_ptr<ruis::image_push_button> clear_button_widget
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
