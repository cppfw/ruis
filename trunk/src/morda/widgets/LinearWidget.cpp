#include "LinearWidget.hpp"
#include "../util/util.hpp"


using namespace morda;



LinearWidget::LinearWidget(const stob::Node* chain) :
		Widget(chain)
{
	if(const stob::Node* n = GetProperty(chain, "vertical")){
		this->isVertical = n->AsBool();
	}else{
		this->isVertical = false;
	}
	
	if(const stob::Node* n = GetProperty(chain, "reverse")){
		this->isReverse = n->AsBool();
	}else{
		this->isReverse = false;
	}
}


