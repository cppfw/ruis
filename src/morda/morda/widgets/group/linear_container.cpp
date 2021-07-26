/*
morda GUI framework
Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

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

#include "linear_container.hpp"

#include "../../util/util.hpp"
#include "../../context.hpp"

#include <cmath>

using namespace morda;

linear_container::layout_params::layout_params(const treeml::forest& desc, const morda::units& units) :
		container::layout_params(desc, units)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "weight"){
			this->weight = get_property_value(p).to_float();
		}
	}
}

std::unique_ptr<widget::layout_params> linear_container::create_layout_params(const treeml::forest& desc)const{
	return std::make_unique<layout_params>(desc, this->context->units);
}

linear_container::linear_container(std::shared_ptr<morda::context> c, const treeml::forest& desc, bool vertical) :
		widget(std::move(c), desc),
		container(this->context, desc),
		oriented_widget(this->context, treeml::forest(), vertical)
{}

namespace{
struct info{
	vector2 measured_dims;
};
}

void linear_container::lay_out(){
	unsigned long_index = this->get_long_index();
	unsigned trans_index = this->get_trans_index();

	std::vector<info> info_array(this->children().size());

	// calculate rigid size, net weight and store weights
	real rigid = 0;
	real net_weight = 0;

	{
		auto info = info_array.begin();
		for(auto i = this->children().cbegin(); i != this->children().cend(); ++i, ++info){
			auto& lp = this->get_layout_params_as_const<layout_params>(**i);

			net_weight += lp.weight;

			ASSERT(lp.dims[long_index] != layout_params::max)
			ASSERT(lp.dims[long_index] != layout_params::fill)

			vector2 d = this->dims_for_widget(**i, lp);
			info->measured_dims = d;

			rigid += d[long_index];
		}
	}

	// arrange widgets
	{
		real flexible = this->rect().d[long_index] - rigid;

		real pos = 0;

		real remainder = 0;

		auto info = info_array.begin();
		for(auto i = this->children().begin(); i != this->children().end(); ++i, ++info){
			auto& lp = this->get_layout_params_as_const<layout_params>(**i);

			if(lp.weight != 0){
				ASSERT(lp.weight > 0)
				vector2 d;
				d[long_index] = info->measured_dims[long_index];
				if(flexible > 0){
					ASSERT(net_weight > 0)
					real dl = flexible * lp.weight / net_weight;
					real floored = std::floor(dl);
					ASSERT(dl >= floored)
					d[long_index] += floored;
					remainder += (dl - floored);
					if(remainder >= real(1)){
						d[long_index] += real(1);
						remainder -= real(1);
					}
				}

				if(lp.dims[trans_index] == layout_params::max || lp.dims[trans_index] == layout_params::fill){
					d[trans_index] = this->rect().d[trans_index];
				}else{
					if(lp.dims[trans_index] == layout_params::min){
						d[trans_index] = -1;
					}else{
						d[trans_index] = lp.dims[trans_index];
					}
					if(d.x() < 0 || d.y() < 0){
						vector2 md = (*i)->measure(d);
						for(unsigned i = 0; i != md.size(); ++i){
							if(d[i] < 0){
								d[i] = md[i];
							}
						}
					}
				}
				(*i)->resize(d);
			}else{
				(*i)->resize(info->measured_dims);
			}

			vector2 newPos;

			newPos[long_index] = pos;

			pos += (*i)->rect().d[long_index];

			newPos[trans_index] = std::round((this->rect().d[trans_index] - (*i)->rect().d[trans_index]) / 2);

			(*i)->move_to(newPos);
		}

		if(remainder > 0){
			vector2 d;
			d[trans_index] = 0;
			d[long_index] = std::round(remainder);
			this->children().back()->resize_by(d);
			this->children().back()->move_by(-d);
		}
	}
}

morda::vector2 linear_container::measure(const morda::vector2& quotum)const{
	unsigned long_index = this->get_long_index();
	unsigned trans_index = this->get_trans_index();

	std::vector<info> info_array(this->children().size());

	// calculate rigid length
	real rigid_length = 0;
	real height = quotum[trans_index] >= 0 ? quotum[trans_index] : 0;
	real net_weight = 0;

	{
		auto info = info_array.begin();
		for(auto i = this->children().begin(); i != this->children().end(); ++i, ++info){
			auto& lp = this->get_layout_params_as_const<layout_params>(**i);

			net_weight += lp.weight;

			if(lp.dims[long_index] == layout_params::max || lp.dims[long_index] == layout_params::fill){
				throw std::logic_error("linear_container::measure(): 'max' or 'fill' in longitudional direction specified in layout parameters");
			}

			vector2 child_quotum;
			if(lp.dims[trans_index] == layout_params::max){
				if(quotum[trans_index] >= 0){
					child_quotum[trans_index] = quotum[trans_index];
				}else{
					child_quotum[trans_index] = -1;
				}
			}else if(lp.dims[trans_index] == layout_params::min){
				child_quotum[trans_index] = -1;
			}else if(lp.dims[trans_index] == layout_params::fill){
				if(quotum[trans_index] >= 0){
					child_quotum[trans_index] = quotum[trans_index];
				}else{
					child_quotum[trans_index] = 0;
				}
			}else{
				child_quotum[trans_index] = lp.dims[trans_index];
			}

			ASSERT(lp.dims[long_index] != layout_params::max)
			ASSERT(lp.dims[long_index] != layout_params::fill)
			if(lp.dims[long_index] == layout_params::min){
				child_quotum[long_index] = -1;
			}else{
				child_quotum[long_index] = lp.dims[long_index];
			}

			info->measured_dims = (*i)->measure(child_quotum);

			rigid_length += info->measured_dims[long_index];

			if(lp.weight == 0){
				if(quotum[trans_index] < 0){
					using std::max;
					height = max(height, info->measured_dims[trans_index]);
				}
			}
		}
	}

	vector2 ret;

	real flex_len;

	if(quotum[long_index] < 0){
		ret[long_index] = rigid_length;
		flex_len = 0;
	}else{
		ret[long_index] = quotum[long_index];
		flex_len = quotum[long_index] - rigid_length;
	}

	{
		real remainder = 0;

		auto last_child = this->children().size() != 0 ? this->children().back().get() : nullptr;

		auto info = info_array.begin();
		for(auto i = this->children().begin(); i != this->children().end(); ++i, ++info){
			auto& lp = this->get_layout_params_as_const<layout_params>(**i);
			ASSERT(lp.weight >= 0)
			if(lp.weight == 0){
				continue;
			}

			ASSERT(net_weight > 0)

			vector2 d;
			d[long_index] = info->measured_dims[long_index];

			if(flex_len > 0){
				real dl = flex_len * lp.weight / net_weight;
				real floored = std::floor(dl);
				ASSERT(dl >= floored)
				d[long_index] += floored;
				remainder += (dl- floored);
				if(remainder >= real(1)){
					d[long_index] += real(1);
					remainder -= real(1);
				}
				if((*i).get() == last_child){
					if(remainder > 0){
						vector2 correction;
						correction[trans_index] = 0;
						correction[long_index] = std::round(remainder);
						d += correction;
					}
				}
			}

			if(lp.dims[trans_index] == layout_params::max){
				if(quotum[trans_index] >= 0){
					d[trans_index] = quotum[trans_index];
				}else{
					d[trans_index] = -1;
				}
			}else if(lp.dims[trans_index] == layout_params::min){
				d[trans_index] = -1;
			}else if(lp.dims[trans_index] == layout_params::fill){
				if(quotum[trans_index] >= 0){
					d[trans_index] = quotum[trans_index];
				}else{
					d[trans_index] = 0;
				}
			}else{
				d[trans_index] = lp.dims[trans_index];
			}

			if(quotum[trans_index] < 0){
				using std::max;
				height = max(height, (*i)->measure(d)[trans_index]);
			}
		}
	}

	ret[trans_index] = height;
	return ret;
}
