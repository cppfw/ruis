#include "OrientedWidget.hpp"

using namespace morda;


OrientedWidget::OrientedWidget(const stob::Node* chain, bool vertical) :
		Widget(chain),
		isVertical_v(vertical)
{

}

void OrientedWidget::setVertical(bool vertical) {
	if(this->isVertical_v == vertical){
		return;
	}
	this->isVertical_v = vertical;
	this->setRelayoutNeeded();
}
