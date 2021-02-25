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
	return utki::make_unique<layout_params>(desc, this->context->units);
}

linear_container::linear_container(std::shared_ptr<morda::context> c, const treeml::forest& desc, bool vertical) :
		widget(std::move(c), desc),
		container(this->context, desc),
		oriented_widget(this->context, treeml::forest(), vertical)
{}

namespace{
class Info{
public:
	vector2 measuredDim;
};
}

void linear_container::lay_out(){
	unsigned longIndex = this->get_long_index();
	unsigned transIndex = this->get_trans_index();

	std::vector<Info> infoArray(this->children().size());

	// calculate rigid size, net weight and store weights
	real rigid = 0;
	real netWeight = 0;

	{
		auto info = infoArray.begin();
		for(auto i = this->children().cbegin(); i != this->children().cend(); ++i, ++info){
			auto& lp = this->get_layout_params_as_const<layout_params>(**i);

			netWeight += lp.weight;

			ASSERT(lp.dims[longIndex] != layout_params::max)
			ASSERT(lp.dims[longIndex] != layout_params::fill)

			vector2 d = this->dims_for_widget(**i, lp);
			info->measuredDim = d;

			rigid += d[longIndex];
		}
	}

	// arrange widgets
	{
		real flexible = this->rect().d[longIndex] - rigid;

		real pos = 0;

		real remainder = 0;

		auto info = infoArray.begin();
		for(auto i = this->children().begin(); i != this->children().end(); ++i, ++info){
			auto& lp = this->get_layout_params_as_const<layout_params>(**i);

			if(lp.weight != 0){
				ASSERT(lp.weight > 0)
				vector2 d;
				d[longIndex] = info->measuredDim[longIndex];
				if(flexible > 0){
					ASSERT(netWeight > 0)
					real dl = flexible * lp.weight / netWeight;
					real floored = std::floor(dl);
					ASSERT(dl >= floored)
					d[longIndex] += floored;
					remainder += (dl - floored);
					if(remainder >= real(1)){
						d[longIndex] += real(1);
						remainder -= real(1);
					}
				}

				if(lp.dims[transIndex] == layout_params::max || lp.dims[transIndex] == layout_params::fill){
					d[transIndex] = this->rect().d[transIndex];
				}else{
					if(lp.dims[transIndex] == layout_params::min){
						d[transIndex] = -1;
					}else{
						d[transIndex] = lp.dims[transIndex];
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
				(*i)->resize(info->measuredDim);
			}

			vector2 newPos;

			newPos[longIndex] = pos;

			pos += (*i)->rect().d[longIndex];

			newPos[transIndex] = std::round((this->rect().d[transIndex] - (*i)->rect().d[transIndex]) / 2);

			(*i)->move_to(newPos);
		}

		if(remainder > 0){
			vector2 d;
			d[transIndex] = 0;
			d[longIndex] = std::round(remainder);
			this->children().back()->resize_by(d);
			this->children().back()->move_by(-d);
		}
	}
}

morda::vector2 linear_container::measure(const morda::vector2& quotum)const{
	unsigned longIndex = this->get_long_index();
	unsigned transIndex = this->get_trans_index();

	std::vector<Info> infoArray(this->children().size());

	// calculate rigid length
	real rigidLength = 0;
	real height = quotum[transIndex] >= 0 ? quotum[transIndex] : 0;
	real netWeight = 0;

	{
		auto info = infoArray.begin();
		for(auto i = this->children().begin(); i != this->children().end(); ++i, ++info){
			auto& lp = this->get_layout_params_as_const<layout_params>(**i);

			netWeight += lp.weight;

			if(lp.dims[longIndex] == layout_params::max || lp.dims[longIndex] == layout_params::fill){
				throw std::logic_error("linear_container::measure(): 'max' or 'fill' in longitudional direction specified in layout parameters");
			}

			vector2 d;
			if(lp.dims[transIndex] == layout_params::max){
				if(quotum[transIndex] >= 0){
					d[transIndex] = quotum[transIndex];
				}else{
					d[transIndex] = -1;
				}
			}else if(lp.dims[transIndex] == layout_params::min){
				d[transIndex] = -1;
			}else if(lp.dims[transIndex] == layout_params::fill){
				if(quotum[transIndex] >= 0){
					d[transIndex] = quotum[transIndex];
				}else{
					d[transIndex] = 0;
				}
			}else{
				d[transIndex] = lp.dims[transIndex];
			}

			ASSERT(lp.dims[longIndex] != layout_params::max)
			ASSERT(lp.dims[longIndex] != layout_params::fill)
			if(lp.dims[longIndex] == layout_params::min){
				d[longIndex] = -1;
			}else{
				d[longIndex] = lp.dims[longIndex];
			}

			d = (*i)->measure(d);
			info->measuredDim = d;

			rigidLength += d[longIndex];

			if(lp.weight == 0){
				if(quotum[transIndex] < 0){
					using std::max;
					height = max(height, d[transIndex]); // clamp bottom
				}
			}
		}
	}

	vector2 ret;

	real flexLen;

	if(quotum[longIndex] < 0){
		ret[longIndex] = rigidLength;
		flexLen = 0;
	}else{
		ret[longIndex] = quotum[longIndex];
		flexLen = quotum[longIndex] - rigidLength;
	}

	{
		real remainder = 0;

		auto lastChild = this->children().size() != 0 ? this->children().back().get() : nullptr;

		auto info = infoArray.begin();
		for(auto i = this->children().begin(); i != this->children().end(); ++i, ++info){
			auto& lp = this->get_layout_params_as_const<layout_params>(**i);
			ASSERT(lp.weight >= 0)
			if(lp.weight == 0){
				continue;
			}

			ASSERT(netWeight > 0)

			vector2 d;
			d[longIndex] = info->measuredDim[longIndex];

			if(flexLen > 0){
				real dl = flexLen * lp.weight / netWeight;
				real floored = std::floor(dl);
				ASSERT(dl >= floored)
				d[longIndex] += floored;
				remainder += (dl- floored);
				if(remainder >= real(1)){
					d[longIndex] += real(1);
					remainder -= real(1);
				}
				if((*i).get() == lastChild){
					if(remainder > 0){
						vector2 correction;
						correction[transIndex] = 0;
						correction[longIndex] = std::round(remainder);
						d += correction;
					}
				}
			}

			if(lp.dims[transIndex] == layout_params::max){
				if(quotum[transIndex] >= 0){
					d[transIndex] = quotum[transIndex];
				}else{
					d[transIndex] = -1;
				}
			}else if(lp.dims[transIndex] == layout_params::min){
				d[transIndex] = -1;
			}else if(lp.dims[transIndex] == layout_params::fill){
				if(quotum[transIndex] >= 0){
					d[transIndex] = quotum[transIndex];
				}else{
					d[transIndex] = 0;
				}
			}else{
				d[transIndex] = lp.dims[transIndex];
			}

			d = (*i)->measure(d);
			if(quotum[transIndex] < 0){
				using std::max;
				height = max(height, d[transIndex]); // clamp bottom
			}
		}
	}

	ret[transIndex] = height;
	return ret;
}
