#include "wire_socket.hpp"

#include "wire_area.hpp"

#include <morda/util/util.hpp>

wire_socket::wire_socket(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
		morda::widget(c, desc)
{
	for(const auto& p : desc){
		if(!morda::is_property(p)){
			continue;
		}

		if(p.value == "outlet"){
			auto v = morda::get_property_value(p);
			if(v == "left"){
				this->outlet_orientation = orientation::left;
			}else if(v == "right"){
				this->outlet_orientation = orientation::right;
			}else if(v == "top"){
				this->outlet_orientation = orientation::top;
			}else if(v == "bottom"){
				this->outlet_orientation = orientation::bottom;
			}
		}
	}
}

void wire_socket::connect(const std::shared_ptr<wire_socket>& o){
	// disconnect existing connection
	this->disconnect();
	
	if(!o || o.get() == this){
		return;
	}
	
	o->disconnect();
	
	this->slave = o;
	this->slave->primary = utki::make_shared_from(*this).to_shared_ptr();
	this->on_connected(*this->slave);
}

void wire_socket::disconnect(){
	if(this->slave){
		ASSERT(!this->primary.lock())
		ASSERT(this->slave->primary.lock().get() == this)
		ASSERT(!this->slave->slave)
		this->slave->primary.reset();
		auto old_slave = std::move(this->slave);
		this->slave.reset();
		this->on_disconnected(*old_slave);
	}else if(auto p = this->primary.lock()){
		ASSERT(!p->primary.lock())
		ASSERT(p->slave.get() == this)
		p->disconnect();
	}
}

std::array<morda::vector2, 2> wire_socket::outlet_pos() const noexcept{
	morda::vector2 dir;
	morda::vector2 pos;
	switch(this->outlet_orientation){
		default:
			ASSERT(false)
		case orientation::bottom:
			pos = this->rect().p + this->rect().d.comp_mul(morda::vector2(0.5, 1));
			dir = morda::vector2(0, 1);
			break;
		case orientation::left:
			pos = this->rect().p + this->rect().d.comp_mul(morda::vector2(0, 0.5));
			dir = morda::vector2(-1, 0);
			break;
		case orientation::right:
			pos = this->rect().p + this->rect().d.comp_mul(morda::vector2(1, 0.5));
			dir = morda::vector2(1, 0);
			break;
		case orientation::top:
			pos = this->rect().p + this->rect().d.comp_mul(morda::vector2(0.5, 0));
			dir = morda::vector2(0, -1);
			break;
	}
	return {{pos, dir}};
}

bool wire_socket::on_mouse_button(const morda::mouse_button_event& e){
	if(e.button != morda::mouse_button::left){
		return false;
	}
	
	if(auto wa = this->try_get_ancestor<wire_area>()){
		if(e.is_down){
			std::shared_ptr<wire_socket> grabbed_socket;
			if(auto p = this->get_remote()){
				p->disconnect();
				grabbed_socket = std::move(p);
			}else{
				grabbed_socket = utki::make_shared_from(*this).to_shared_ptr();
			}
			
			wa->grabbed_socket = std::move(grabbed_socket);
			wa->mouse_pos = this->pos_in_ancestor(e.pos, wa);
		}else{
			wa->grabbed_socket->connect(wa->hovered_socket);
			wa->grabbed_socket.reset();
		}
		return true;
	}
	return false;
}

void wire_socket::on_hover_change(unsigned pointer_id){
	// LOG("Hover changed: " << this->is_hovered(pointer_id) << std::endl)
	if(auto wa = this->try_get_ancestor<wire_area>()){
		if(this->is_hovered()){
			wa->hovered_socket = utki::make_shared_from(*this).to_shared_ptr();
		}else{
			if(wa->hovered_socket.get() == this){
				wa->hovered_socket.reset();
			}
		}
	}
}

std::shared_ptr<wire_socket> wire_socket::get_remote(){
	if(this->slave){
		return this->slave;
	}
	
	if(auto p = this->primary.lock()){
		return p;
	}
	
	return nullptr;
}
