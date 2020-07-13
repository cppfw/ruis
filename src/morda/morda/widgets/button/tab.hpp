#pragma once

#include "../label/nine_patch.hpp"
#include "choice_button.hpp"
#include "nine_patch_toggle.hpp"

namespace morda{

class tab :
		virtual public choice_button,
		public nine_patch_toggle
{
	bool maskOverlaps(vector2 pos);
	
public:
	tab(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
	tab(const tab&) = delete;
	tab& operator=(const tab&) = delete;
	
protected:	
	bool on_mouse_button(const mouse_button_event& event)override;

	void on_hover_change(unsigned pointer_id)override{}
	
	void on_press_change()override;
};

}
