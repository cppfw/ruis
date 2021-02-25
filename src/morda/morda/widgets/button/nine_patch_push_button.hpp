#pragma once

#include "push_button.hpp"
#include "nine_patch_button.hpp"

namespace morda{

class nine_patch_push_button :
		virtual public push_button,
		public nine_patch_button
{
public:
	nine_patch_push_button(std::shared_ptr<morda::context> c, const treeml::forest& desc);
	
protected:
	void on_press_change()override;
	
	bool on_mouse_button(const mouse_button_event& e)override{
		return this->push_button::on_mouse_button(e);
	}

	void on_hover_change(unsigned pointerID)override{
		this->push_button::on_hover_change(pointerID);
	}
};

}
