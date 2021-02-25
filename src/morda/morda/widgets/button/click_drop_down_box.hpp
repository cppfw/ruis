#pragma once

#include "drop_down_box.hpp"
#include "nine_patch_push_button.hpp"

namespace morda{

class click_drop_down_box :
		virtual public widget,
		private nine_patch_push_button,
		public drop_down_box
{
	std::weak_ptr<widget> current_drop_down_menu;

	unsigned num_mouse_buttons_pressed = 0;
public:
	click_drop_down_box(std::shared_ptr<morda::context> c, const treeml::forest& desc);

	click_drop_down_box(const click_drop_down_box&) = delete;
	click_drop_down_box& operator=(const click_drop_down_box&) = delete;

private:
	// index of the hovered item in the drop down menu
	int hovered_index = -1;

	bool on_mouse_button(const mouse_button_event& e)override;
	bool on_mouse_move(const mouse_move_event& e)override;

	std::shared_ptr<widget> wrap_item(std::shared_ptr<widget>&& w, size_t index);

	void show_drop_down_menu();

	void handle_mouse_button_up(bool is_first_button_up_event);
};

}
