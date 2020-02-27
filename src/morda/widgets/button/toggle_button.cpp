#include "toggle_button.hpp"

#include "../../util/util.hpp"

using namespace morda;

toggle_button::toggle_button(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc)
{}

bool toggle_button::on_mouse_button(bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerID){
	if(button == mouse_button::left){
		if(isDown){
			this->toggle();
		}
		return true;
	}

	return false;
}
