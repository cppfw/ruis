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
