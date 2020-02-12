#include "ColorWidget.hpp"

#include "../../util/util.hpp"

using namespace morda;


ColorWidget::ColorWidget(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc)
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
