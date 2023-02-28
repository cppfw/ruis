/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#include "pile.hpp"

#include "../../util/util.hpp"

using namespace morda;

pile::pile(const utki::shared_ref<morda::context>& c, const treeml::forest& desc) :
		widget(c, desc),
		container(this->context, desc)
{}

void pile::lay_out(){
//	TRACE(<< "pile::lay_out(): invoked" << std::endl)
	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		auto& lp = i->get().get_layout_params_const();

		i->get().resize(this->dims_for_widget(i->get(), lp));

		using std::round;
		i->get().move_to(round((this->rect().d - i->get().rect().d) / 2));
	}
}

morda::vector2 pile::measure(const morda::vector2& quotum)const{
	vector2 ret(quotum);
	using std::max;
	ret = max(ret, real(0)); // clamp bottom

	for(auto i = this->children().begin(); i != this->children().end(); ++i){
		auto& lp = i->get().get_layout_params_const();

		morda::vector2 d;

		for(unsigned j = 0; j != d.size(); ++j){
			if(lp.dims[j] == layout_params::max){
				if(quotum[j] >= 0){
					d[j] = quotum[j];
				}else{
					d[j] = -1;
				}
			}else if(lp.dims[j] == layout_params::min){
				d[j] = -1;
			}else if(lp.dims[j] == layout_params::fill){
				if(quotum[j] >= 0){
					d[j] = quotum[j];
				}else{
					d[j] = 0;
				}
			}else{
				d[j] = lp.dims[j];
			}
		}

		d = i->get().measure(d);

		for(unsigned j = 0; j != d.size(); ++j){
			if(quotum[j] < 0){
				using std::max;
				ret[j] = max(ret[j], d[j]); // clamp bottom
			}
		}
	}

	return ret;
}
