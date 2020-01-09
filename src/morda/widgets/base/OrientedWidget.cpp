#include "OrientedWidget.hpp"

using namespace morda;


OrientedWidget::OrientedWidget(const puu::trees& desc, bool vertical) :
		widget(desc),
		isVertical_v(vertical)
{}

void OrientedWidget::setVertical(bool vertical) {
	if(this->isVertical_v == vertical){
		return;
	}
	this->isVertical_v = vertical;
	this->invalidate_layout();
}
