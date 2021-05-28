#include "choice_button.hpp"

#include "choice_group.hpp"

using namespace morda;

choice_button::choice_button(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		button(this->context, desc),
		toggle_button(this->context, desc)
{}

void choice_button::on_press_change(){
	this->toggle_button::on_press_change();

	auto cg = this->try_get_ancestor<choice_group>();
	if(!cg){
		return;
	}
	
	if(this->is_pressed()){
		cg->setActiveChoiceButton(utki::make_shared_from(*this));
	}
}

bool choice_button::on_mouse_button(const mouse_button_event& e){
	if(this->is_pressed()){
		return true;
	}
	return this->toggle_button::on_mouse_button(e);
}
