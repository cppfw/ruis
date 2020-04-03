#pragma once

#include "drop_down_box.hpp"
#include "nine_patch_push_button.hpp"

namespace morda{

class click_drop_down_box :
		virtual public widget,
		private nine_patch_push_button,
		public drop_down_box
{
public:
	click_drop_down_box(std::shared_ptr<morda::context> c, const puu::forest& desc);

	click_drop_down_box(const click_drop_down_box&) = delete;
	click_drop_down_box& operator=(const click_drop_down_box&) = delete;

private:
	// index of the hovered item in the drop down menu
	int hovered_index = -1;

	bool on_mouse_button(bool is_down, const morda::vector2& pos, mouse_button button, unsigned pointer_id)override;

	std::shared_ptr<widget> wrap_item(std::shared_ptr<widget>&& w, size_t index);

	void show_drop_down_menu();

	void mouse_button_up_handler(bool is_first_button_up_event);
};

}
