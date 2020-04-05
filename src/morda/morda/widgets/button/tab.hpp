#pragma once

#include "../label/nine_patch.hpp"
#include "choice_button.hpp"
#include "nine_patch_toggle.hpp"

namespace morda{

class tab :
		public choice_button,
		public nine_patch_toggle
{
	bool maskOverlaps(vector2 pos);
	
public:
	tab(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
	tab(const tab&) = delete;
	tab& operator=(const tab&) = delete;
	
protected:	
	bool on_mouse_button(bool isDown, const morda::vector2& pos, mouse_button button, unsigned pointerId)override;

	void on_hover_changed(unsigned pointer_id)override{}
	
	void on_pressed_changed()override;
};

}
