#include "toggle_button.hpp"

#include "../../util/util.hpp"

using namespace morda;

toggle_button::toggle_button(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc)
{}

bool toggle_button::on_mouse_button(const mouse_button_event& e){
	if(e.button == mouse_button::left){
		if(e.is_down){
			this->toggle();
		}
		return true;
	}

	return false;
}
