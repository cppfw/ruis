/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "scroll_area.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"

using namespace morda;

scroll_area::scroll_area(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		container(nullptr, desc)
{}

bool scroll_area::on_mouse_button(const mouse_button_event& e){
	vector2 d = -this->cur_scroll_pos;
	return this->container::on_mouse_button(mouse_button_event{
			e.is_down,
			e.pos - d,
			e.button,
			e.pointer_id
		});
}

bool scroll_area::on_mouse_move(const mouse_move_event& e){
	vector2 d = -this->cur_scroll_pos;
	return this->container::on_mouse_move(mouse_move_event{
			e.pos - d,
			e.pointer_id,
			e.ignore_mouse_capture
		});
}

void scroll_area::render(const morda::matrix4& matrix)const{
	vector2 d = -this->cur_scroll_pos;

	matrix4 matr(matrix);
	matr.translate(d);

	this->container::render(matr);
}

void scroll_area::clamp_scroll_pos(){
	if(this->invisible_dims.x() < 0){
		this->cur_scroll_pos.x() = 0;
	}

	if(this->invisible_dims.y() < 0){
		this->cur_scroll_pos.y() = 0;
	}
}

void scroll_area::set_scroll_pos(const vector2& new_scroll_pos){
	using std::round;
	this->cur_scroll_pos = round(new_scroll_pos);

	this->clamp_scroll_pos();
	this->update_scroll_factor();
	
	this->on_scroll_pos_change();
}

void scroll_area::set_scroll_factor(const vector2& factor){
	vector2 new_scroll_pos = this->invisible_dims.comp_mul(factor);

	this->set_scroll_pos(new_scroll_pos);
}

void scroll_area::update_scroll_factor(){
	// at this point effective dimension should be updated
	vector2 factor = this->cur_scroll_pos.comp_div(this->invisible_dims);

	if(this->cur_scroll_factor == factor){
		return;
	}

	for(unsigned i = 0; i != 2; ++i){
		if(this->invisible_dims[i] <= 0){
			this->cur_scroll_factor[i] = 0;
		}else{
			this->cur_scroll_factor[i] = this->cur_scroll_pos[i] / this->invisible_dims[i];
		}
	}
}

// TODO: why is there special dims_for_widget() and why container::dims_for_widget() is not like this? 
vector2 scroll_area::dims_for_widget(const widget& w, const layout_params& lp)const{
	vector2 d;
	for(unsigned i = 0; i != 2; ++i){
		if(lp.dims[i] == layout_params::fill){
			d[i] = this->rect().d[i];
		}else if(lp.dims[i] == layout_params::min || lp.dims[i] == layout_params::max){
			d[i] = -1; // will be updated below
		}else{
			d[i] = lp.dims[i];
		}
	}
	if(!d.is_positive_or_zero()){
		vector2 md = w.measure(d);
		for(unsigned i = 0; i != md.size(); ++i){
			if(d[i] < 0){
				if(lp.dims[i] == layout_params::max && md[i] < this->rect().d[i]){
					d[i] = this->rect().d[i];
				}else{
					d[i] = md[i];
				}
			}
		}
	}
	return d;
}

void scroll_area::arrange_widgets(){
	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		auto& lp = this->get_layout_params_const(**i);

		auto d = this->dims_for_widget(**i, lp);

		(*i)->resize(d);
	}
}

void scroll_area::lay_out(){
	this->arrange_widgets();
	this->update_effective_dims();

	// distance of content's bottom right corner from bottom right corner of the scroll_area
	vector2 br = this->cur_scroll_pos - this->invisible_dims;

	if(br.x() > 0){
		if(br.x() <= this->cur_scroll_pos.x()){
			this->cur_scroll_pos.x() -= br.x();
		}else{
			this->cur_scroll_pos.x() = 0;
		}
	}

	if(br.y() > 0){
		if(br.y() <= this->cur_scroll_pos.y()){
			this->cur_scroll_pos.y() -= br.y();
		}else{
			this->cur_scroll_pos.y() = 0;
		}
	}

	this->context->run_from_ui_thread([sa = utki::make_weak_from(*this)](){
		if(auto s = sa.lock()){
			s->on_scroll_pos_change();
		}
	});
}

void scroll_area::on_children_change(){
	this->container::on_children_change();
	this->arrange_widgets();
	this->update_effective_dims();
}

void scroll_area::update_effective_dims(){
	morda::vector2 minDim(0);

	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		auto& lp = this->get_layout_params_as_const<layout_params>(**i);

		morda::vector2 d = (*i)->rect().p + this->dims_for_widget(**i, lp);

		using std::max;

		minDim = max(minDim, d); // clamp bottom
	}

	this->invisible_dims = minDim - this->rect().d;
	this->update_scroll_factor();
}

vector2 scroll_area::get_visible_area_fraction()const noexcept{
	auto ret = this->rect().d.comp_div(this->rect().d + this->invisible_dims);

	using std::min;
	ret = min(ret, real(1)); // clamp top

	return ret;
}

void scroll_area::on_scroll_pos_change(){
	if(this->scroll_change_handler){
		this->scroll_change_handler(*this);
	}
}
