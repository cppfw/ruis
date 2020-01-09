#include "ColorWidget.hpp"

#include "../../util/util.hpp"

using namespace morda;


ColorWidget::ColorWidget(const puu::forest& desc) :
		widget(desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "color"){
			this->color_v = get_property_value(p).to_uint32();
		}
	}
}
