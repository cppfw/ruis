#include "click_proxy.hpp"

using namespace morda;

click_proxy::click_proxy(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
        widget(std::move(c), desc)
{}

bool click_proxy::on_mouse_button(const mouse_button_event& e){
    bool ret = false;

    if(e.button != mouse_button::left){
        return false;
    }

    if(e.pointer_id != 0){
        return false;
    }

    if(e.is_down){
        this->is_pressed_ = true;
        if(this->press_change_handler){
            ret = this->press_change_handler(*this);
        }
    }else{
        if(this->is_pressed()){
            this->is_pressed_ = false;
            if(this->press_change_handler){
                ret = this->press_change_handler(*this);
            }
            if(this->click_handler){
                this->click_handler(*this);
            }
        }else{
            ret = this->deferred_release_ret;
        }
    }

	return ret;
}

void click_proxy::on_hover_change(unsigned pointer_id){
    // TRACE(<< "on_hover_change(): hovered = " << this->is_hovered() << " pointer_id = " << pointer_id << std::endl)
    if(pointer_id != 0){
        return;
    }

	if(!this->is_hovered(0)){
        if(this->is_pressed()){
            this->is_pressed_ = false;
            if(this->press_change_handler){
                this->deferred_release_ret = this->press_change_handler(*this);
            }
        }
    }
}
