#include "choice_button.hpp"

#include "choice_group.hpp"

using namespace morda;

choice_button::choice_button(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		ToggleButton(this->context, desc)
{}

void choice_button::on_pressed_changed(){
	auto cg = this->find_ancestor<choice_group>();
	if(!cg){
		this->ToggleButton::on_pressed_changed();
		return;
	}
	
	this->ToggleButton::on_pressed_changed();
	
	if(this->is_pressed()){
		cg->setActiveChoiceButton(this->sharedFromThis(this));
	}
}

bool choice_button::on_mouse_button(bool isDown, const morda::Vec2r& pos, mouse_button button, unsigned pointerID){
	if(this->is_pressed()){
		return true;
	}
	return this->ToggleButton::on_mouse_button(isDown, pos, button, pointerID);
}
