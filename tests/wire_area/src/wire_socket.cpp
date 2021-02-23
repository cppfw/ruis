#include "wire_socket.hpp"

#include "wire_area.hpp"

#include <morda/util/util.hpp>

wire_socket::wire_socket(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		morda::widget(std::move(c), desc)
{
	for(const auto& p : desc){
		if(!morda::is_property(p)){
			continue;
		}

		if(p.value == "outlet"){
			auto v = morda::get_property_value(p);
			if(v == "left"){
				this->outlet_v = Outlet_e::LEFT;
			}else if(v == "right"){
				this->outlet_v = Outlet_e::RIGHT;
			}else if(v == "top"){
				this->outlet_v = Outlet_e::TOP;
			}else if(v == "bottom"){
				this->outlet_v = Outlet_e::BOTTOM;
			}
		}
	}
}

void wire_socket::connect(const std::shared_ptr<wire_socket>& o) {
	// disconnect existing connection
	this->disconnect();
	
	if(!o || o.get() == this){
		return;
	}
	
	o->disconnect();
	
	this->slave = o;
	this->slave->primary = utki::make_shared_from(*this);
	this->onConnected(*this->slave);
}

void wire_socket::disconnect() {
	if(this->slave){
		ASSERT(!this->primary.lock())
		ASSERT(this->slave->primary.lock().get() == this)
		ASSERT(!this->slave->slave)
		this->slave->primary.reset();
		auto oldSlave = std::move(this->slave);
		this->slave.reset();
		this->onDisconnected(*oldSlave);
	}else if(auto p = this->primary.lock()){
		ASSERT(!p->primary.lock())
		ASSERT(p->slave.get() == this)
		p->disconnect();
	}
}



std::array<morda::vector2, 2> wire_socket::outletPos() const noexcept{
	morda::vector2 dir;
	morda::vector2 pos;
	switch(this->outlet_v){
		default:
			ASSERT(false)
		case Outlet_e::BOTTOM:
			pos = this->rect().p + this->rect().d.comp_mul(morda::vector2(0.5, 1));
			dir = morda::vector2(0, 1);
			break;
		case Outlet_e::LEFT:
			pos = this->rect().p + this->rect().d.comp_mul(morda::vector2(0, 0.5));
			dir = morda::vector2(-1, 0);
			break;
		case Outlet_e::RIGHT:
			pos = this->rect().p + this->rect().d.comp_mul(morda::vector2(1, 0.5));
			dir = morda::vector2(1, 0);
			break;
		case Outlet_e::TOP:
			pos = this->rect().p + this->rect().d.comp_mul(morda::vector2(0.5, 0));
			dir = morda::vector2(0, -1);
			break;
	}
	return {{pos, dir}};
}

bool wire_socket::on_mouse_button(const morda::mouse_button_event& e) {
	if(e.button != morda::mouse_button::left){
		return false;
	}
	
	if(auto wa = this->find_ancestor<wire_area>()){
		if(e.is_down){
			std::shared_ptr<wire_socket> grabbedSocket;
			if(auto p = this->getRemote()){
				p->disconnect();
				grabbedSocket = std::move(p);
			}else{
				grabbedSocket = utki::make_shared_from(*this);
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

void wire_socket::on_hover_change(unsigned pointerID) {
//	TRACE(<< "Hover changed: " << this->isHovered() << " " << this << std::endl)
	if(auto wa = this->find_ancestor<wire_area>()){
		if(this->is_hovered()){
			wa->hoveredSocket = utki::make_shared_from(*this);
		}else{
			if(wa->hoveredSocket.get() == this){
				wa->hoveredSocket.reset();
			}
		}
	}
}

std::shared_ptr<wire_socket> wire_socket::getRemote() {
	if(this->slave){
		return this->slave;
	}
	
	if(auto p = this->primary.lock()){
		return p;
	}
	
	return nullptr;
}
