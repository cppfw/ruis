#include "ToggleButton.hpp"

#include "../../util/util.hpp"

using namespace morda;


ToggleButton::ToggleButton(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		Button(this->context, desc)
{
}

bool ToggleButton::on_mouse_button(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerID) {
	if(button == MouseButton_e::LEFT){
		if(isDown){
			this->toggle();
		}
		return true;
	}

	return false;
}
