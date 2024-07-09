#include "wire_area.hpp"

#include <ruis/paint/path_vao.hpp>
#include <ruis/context.hpp>
#include <ruis/util/util.hpp>
#include <ruis/layout/pile_layout.hpp>

namespace{
const ruis::real antialias_width = ruis::real(0.55f);
const ruis::real spline_control_length = ruis::real(100);
}

wire_area::wire_area(const utki::shared_ref<ruis::context>& c, const tml::forest& desc) :
		widget(c, desc),
		container(this->context, desc, ruis::layout::trivial)
{
	for(const auto& p : desc){
		if(!ruis::is_property(p)){
			continue;
		}

		if(p.value == "wire_width"){
			this->wire_half_width = ruis::get_property_value(p).to_float() / 2;
		}else if(p.value == "wire_color"){
			this->wire_color = ruis::get_property_value(p).to_uint32();
		}else if(p.value == "grabbed_color"){
			this->grabbed_color = ruis::get_property_value(p).to_uint32();
		}
	}
}

void wire_area::render(const ruis::matrix4& matrix)const{
	this->container::render(matrix);
	
	for(auto& s : this->sockets){
		if(!s.get().slave){
			continue;
		}
		
		auto prim_outlet_pos = s.get().outlet_pos();
		auto slave_outlet_pos = s.get().slave->outlet_pos();
		auto p0 = s.get().pos_in_ancestor(prim_outlet_pos[0], this);
		auto p = s.get().slave->pos_in_ancestor(slave_outlet_pos[0], this) - p0;
		
		ruis::path path;
		path.cubic_to(prim_outlet_pos[1] * spline_control_length, p + slave_outlet_pos[1] * spline_control_length, p);
		
		ruis::path_vao vba(this->context.get().renderer);
		vba.set(path.stroke(this->wire_half_width, antialias_width, 1));
		
		vba.render(ruis::matrix4(matrix).translate(p0), this->wire_color);
	}
	
	if(this->grabbed_socket){
		auto outlet_pos = this->grabbed_socket->outlet_pos();
		auto p0 = this->grabbed_socket->pos_in_ancestor(outlet_pos[0], this);
		
		ruis::path path;
		path.line_to(this->mouse_pos - p0);
		
		ruis::path_vao vba(this->context.get().renderer);
		vba.set(path.stroke(this->wire_half_width, antialias_width, 1));
		
		vba.render(ruis::matrix4(matrix).translate(p0), this->grabbed_color);
	}
}

bool wire_area::on_mouse_move(const ruis::mouse_move_event& e){
	// LOG("mm" << std::endl)
	if(this->grabbed_socket){
		this->mouse_pos = e.pos;
		return this->container::on_mouse_move(
				ruis::mouse_move_event{
					e.pos,
					e.pointer_id,
					true
				}
			);
	}
	return this->container::on_mouse_move(e);
}

void wire_area::on_lay_out(){
	this->container::on_lay_out();
	
	this->sockets = this->get_all_widgets<wire_socket>();
	// TRACE(<< "this->sockets.size() = " << this->sockets.size() << std::endl)
}
