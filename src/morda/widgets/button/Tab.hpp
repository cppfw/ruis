#pragma once

#include "../label/NinePatch.hpp"
#include "ChoiceButton.hpp"
#include "NinePatchToggle.hpp"

namespace morda{
class Tab :
		public ChoiceButton,
		public NinePatchToggle
{
	bool maskOverlaps(Vec2r pos);
	
public:
	Tab(std::shared_ptr<morda::context> c, const puu::forest& desc);
	
	Tab(const Tab&) = delete;
	Tab& operator=(const Tab&) = delete;
	
protected:	
	bool on_mouse_button(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerId) override;

	void on_hover_changed(unsigned pointerID) override{}
	
	void onPressedChanged() override;
};
}
