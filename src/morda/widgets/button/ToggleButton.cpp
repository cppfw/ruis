#include "ToggleButton.hpp"

#include "../../util/util.hpp"

using namespace morda;


ToggleButton::ToggleButton(const stob::Node* chain) :
		Widget(chain),
		Button(chain)
{
}

bool ToggleButton::onMouseButton(bool isDown, const morda::Vec2r& pos, MouseButton_e button, unsigned pointerID) {
	if(button == MouseButton_e::LEFT){
		if(isDown){
			this->toggle();
		}
		return true;
	}

	return false;
}
