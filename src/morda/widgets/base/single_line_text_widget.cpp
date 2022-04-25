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

#include "single_line_text_widget.hpp"

using namespace morda;

single_line_text_widget::single_line_text_widget(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc),
		text_widget(this->context, desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "text"){
			this->text = utki::to_utf32(get_property_value(p).to_string());
			this->recompute_bounding_box();
		}
	}
}

void single_line_text_widget::recompute_bounding_box(){
	this->bb = this->get_font().get().get_bounding_box(this->single_line_text_widget::get_text());
}

vector2 single_line_text_widget::measure(const morda::vector2& quotum)const noexcept{
	vector2 ret(this->bb.d.x(), this->get_font().get().get_height());

	for(unsigned i = 0; i != ret.size(); ++i){
		if(quotum[i] >= 0){
			ret[i] = quotum[i];
		}
	}

	return ret;
}

void single_line_text_widget::on_text_change(){
	this->recompute_bounding_box();
	this->text_widget::on_text_change();
}

void single_line_text_widget::set_text(std::u32string&& text){
	this->text = std::move(text);
	this->invalidate_layout();
	this->on_text_change();
}

std::u32string single_line_text_widget::get_text()const{
	return this->text;
}
