#include "oriented_widget.hpp"

using namespace morda;

oriented_widget::oriented_widget(std::shared_ptr<morda::context> c, const puu::forest& desc, bool vertical) :
		widget(std::move(c), desc),
		is_vertical_v(vertical)
{}

void oriented_widget::set_vertical(bool vertical) {
	if(this->is_vertical_v == vertical){
		return;
	}
	this->is_vertical_v = vertical;
	this->invalidate_layout();
}
