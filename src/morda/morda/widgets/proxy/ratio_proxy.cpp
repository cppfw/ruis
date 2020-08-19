#include "ratio_proxy.hpp"

using namespace morda;

ratio_proxy::ratio_proxy(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		morda::widget(std::move(c), desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "ratio"){
			this->x_above_y = get_property_value(p).to_float();
		}
	}
}

void ratio_proxy::set_aspect_ratio(real x_above_y){
	if(this->x_above_y == x_above_y){
		return;
	}

	this->x_above_y = x_above_y;
	this->invalidate_layout();
}

morda::vector2 ratio_proxy::measure(const morda::vector2& quotum)const{
	if(quotum.x < 0 && quotum.y < 0){
		return morda::vector2(0);
	}

	if(quotum.x >=0 && quotum.y < 0){
		return morda::vector2(quotum.x, quotum.x / this->x_above_y);
	}

	return morda::vector2(quotum.y * this->x_above_y, quotum.y);
}
