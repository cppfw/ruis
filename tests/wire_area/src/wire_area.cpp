#include "wire_area.hpp"

#include <morda/paint/path_vao.hpp>

#include <morda/context.hpp>
#include <morda/util/util.hpp>

namespace{
const morda::real antialiasWidth_c = morda::real(0.55f);
const morda::real splineControlLength_c = morda::real(100);
}

wire_area::wire_area(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		size_container(this->context, desc)
{
	for(const auto& p : desc){
		if(!morda::is_property(p)){
			continue;
		}

		if(p.value == "wireWidth"){
			this->wireHalfWidth = morda::get_property_value(p).to_float() / 2;
		}else if(p.value == "wireColor"){
			this->wireColor = morda::get_property_value(p).to_uint32();
		}else if(p.value == "grabbedColor"){
			this->grabbedColor = morda::get_property_value(p).to_uint32();
		}
	}
}


void wire_area::render(const morda::matrix4& matrix) const {
	this->container::render(matrix);
	
	for(auto& s : this->sockets){
		if(!s->slave){
			continue;
		}
		
		auto primOutletPos = s->outletPos();
		auto slaveOutletPos = s->slave->outletPos();
		auto p0 = s->pos_in_ancestor(primOutletPos[0], this);
		auto p = s->slave->pos_in_ancestor(slaveOutletPos[0], this) - p0;
		
		morda::path path;
		path.cubic_to(primOutletPos[1] * splineControlLength_c, p + slaveOutletPos[1] * splineControlLength_c, p);
		
		morda::path_vao vba(this->context->renderer, path.stroke(this->wireHalfWidth, antialiasWidth_c, 1));
		
		vba.render(morda::matrix4(matrix).translate(p0), this->wireColor);
	}
	
	if(this->grabbedSocket){
		auto outletPos = this->grabbedSocket->outletPos();
		auto p0 = this->grabbedSocket->pos_in_ancestor(outletPos[0], this);
		
		morda::path path;
		path.line_to(mousePos - p0);
		
		morda::path_vao vba(this->context->renderer, path.stroke(this->wireHalfWidth, antialiasWidth_c, 1));
		
		vba.render(morda::matrix4(matrix).translate(p0), this->grabbedColor);
	}
}

bool wire_area::on_mouse_move(const morda::mouse_move_event& e) {
	if(this->grabbedSocket){
		this->mousePos = e.pos;
	}
	return this->container::on_mouse_move(e);
}

void wire_area::lay_out() {
	this->size_container::lay_out();
	
	this->sockets = this->get_all_widgets<wire_socket>();
	// TRACE(<< "this->sockets.size() = " << this->sockets.size() << std::endl)
}
