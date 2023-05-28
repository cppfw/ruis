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
		auto oldSlave = std::move(this->slave);
		this->slave.reset();
		this->on_disconnected(*oldSlave);
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
			std::shared_ptr<wire_socket> grabbedSocket;
			if(auto p = this->get_remote()){
				p->disconnect();
				grabbedSocket = std::move(p);
			}else{
				grabbedSocket = utki::make_shared_from(*this).to_shared_ptr();
			}
			
			wa->grabbedSocket = std::move(grabbedSocket);
			wa->mousePos = this->pos_in_ancestor(e.pos, wa);
		}else{
			wa->grabbedSocket->connect(wa->hoveredSocket);
			wa->grabbedSocket.reset();
		}
		return true;
	}
	return false;
}

void wire_socket::on_hover_change(unsigned pointer_id){
	// LOG("Hover changed: " << this->is_hovered(pointer_id) << std::endl)
	if(auto wa = this->try_get_ancestor<wire_area>()){
		if(this->is_hovered()){
			wa->hoveredSocket = utki::make_shared_from(*this).to_shared_ptr();
		}else{
			if(wa->hoveredSocket.get() == this){
				wa->hoveredSocket.reset();
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
