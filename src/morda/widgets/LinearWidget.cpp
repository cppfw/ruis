#include "LinearWidget.hpp"
#include "../util/util.hpp"


using namespace morda;



LinearWidget::LinearWidget(const stob::Node* desc) :
		Widget(desc)
{
	if(const stob::Node* n = GetProperty(desc, "vertical")){
		this->isVertical = n->AsBool();
	}else{
		this->isVertical = false;
	}
	
	if(const stob::Node* n = GetProperty(desc, "reverse")){
		this->isReverse = n->AsBool();
	}else{
		this->isReverse = false;
	}
}


