#include "button.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"


using namespace morda;

button::button(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "pressed"){
			this->isPressed_v = get_property_value(p).to_bool();
		}
	}
}


void button::set_pressed(bool pressed){
	if(this->isPressed_v == pressed){
		return;
	}
	this->isPressed_v = pressed;
	this->isPressedChangedNotified = false;
	this->on_pressed_changed();
}



void button::on_pressed_changed(){
	if(this->isPressedChangedNotified){
		return;
	}
	this->isPressedChangedNotified = true;
	
	if (this->pressed_changed) {
		this->pressed_changed(*this);
	}
}
