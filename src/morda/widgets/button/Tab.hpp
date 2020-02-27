#pragma once

#include "../label/NinePatch.hpp"
#include "choice_button.hpp"
#include "NinePatchToggle.hpp"

namespace morda{
class Tab :
		public choice_button,
		public NinePatchToggle
{
	bool maskOverlaps(Vec2r pos);
	
public:
	Tab(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
	Tab(const Tab&) = delete;
	Tab& operator=(const Tab&) = delete;
	
protected:	
	bool on_mouse_button(bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerId)override;

	void on_hover_changed(unsigned pointer_id)override{}
	
	void on_pressed_changed()override;
};
}
