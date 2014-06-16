#include "LinearWidget.hpp"


using namespace morda;



LinearWidget::LinearWidget(const stob::Node& desc) {
	if(const stob::Node* n = desc.GetProperty("vertical")){
		this->isVertical = n->AsBool();
	}
	
	if(const stob::Node* n = desc.GetProperty("reverse")){
		this->isReverse = n->AsBool();
	}
}


