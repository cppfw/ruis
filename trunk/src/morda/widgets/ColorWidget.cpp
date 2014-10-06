#include "ColorWidget.hpp"

#include "../util/util.hpp"

using namespace morda;



ColorWidget::ColorWidget(const stob::Node* chain) :
		Widget(chain)
{
	if(const stob::Node* n = GetProperty(chain, "color")){
		this->color = n->AsUint32();
	}else{
		this->color = 0xffffffff;
	}
}
