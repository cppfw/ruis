#include "click_proxy.hpp"

using namespace morda;

click_proxy::click_proxy(std::shared_ptr<morda::context> c, const puu::forest& desc) :
        widget(std::move(c), desc)
{}

bool click_proxy::on_mouse_button(bool is_down, const morda::vector2& pos, mouse_button button, unsigned pointer_id){
    bool ret = false;

    if(button != mouse_button::left){
        return false;
    }

    if(pointer_id != 0){
        return false;
    }

    if(is_down){
        this->is_pressed = true;
        if(this->press_handler){
            ret = this->press_handler(*this, true);
        }
    }else{
        if(this->is_pressed){
            this->is_pressed = false;
            if(this->press_handler){
                ret = this->press_handler(*this, false);
            }
            if(this->click_handler){
                ret |= this->click_handler(*this);
            }
        }
    }

	return ret;
}

void click_proxy::on_hover_changed(unsigned pointer_id){
    // TRACE(<< "on_hover_changed(): hovered = " << this->is_hovered() << " pointer_id = " << pointer_id << std::endl)
    if(pointer_id != 0){
        return;
    }

	if(!this->is_hovered(0)){
        if(this->is_pressed){
            this->is_pressed = false;
            if(this->press_handler){
                this->press_handler(*this, false);
            }
        }
    }
}
