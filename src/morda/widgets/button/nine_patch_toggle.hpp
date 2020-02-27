#pragma once

#include "toggle_button.hpp"
#include "nine_patch_button.hpp"

namespace morda{

class nine_patch_toggle :
		virtual public toggle_button,
		public nine_patch_button
{
public:
	nine_patch_toggle(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
protected:
	void on_pressed_changed()override;

	bool on_mouse_button(bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerID)override{
		return this->toggle_button::on_mouse_button(isDown, pos, button, pointerID);
	}
};

}
