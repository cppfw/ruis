#include "color_widget.hpp"

#include "../../util/util.hpp"

using namespace morda;

color_widget::color_widget(std::shared_ptr<morda::context> c, const treeml::forest& desc) :
		widget(std::move(c), desc)
{
	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "color"){
			this->color = get_property_value(p).to_uint32();
		}else if(p.value == "disabled_color"){
			this->disabled_color = get_property_value(p).to_uint32();
		}
	}
}

void color_widget::set_color(uint32_t color){
	if(this->color == color){
		return;
	}

	this->color = color;
	this->clear_cache();
}

void color_widget::set_disabled_color(uint32_t color){
	if(this->disabled_color == color){
		return;
	}

	this->disabled_color = color;
	this->clear_cache();
}

uint32_t color_widget::get_current_color()const noexcept{
	if(this->is_enabled()){
		return this->get_color();
	}else{
		return this->get_disabled_color();
	}
}
