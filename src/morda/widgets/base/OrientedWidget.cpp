#include "OrientedWidget.hpp"

using namespace morda;


OrientedWidget::OrientedWidget(const std::shared_ptr<morda::context>& c, const puu::forest& desc, bool vertical) :
		widget(c, desc),
		isVertical_v(vertical)
{}

void OrientedWidget::setVertical(bool vertical) {
	if(this->isVertical_v == vertical){
		return;
	}
	this->isVertical_v = vertical;
	this->invalidate_layout();
}
