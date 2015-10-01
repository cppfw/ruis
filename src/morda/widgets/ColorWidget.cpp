#include "ColorWidget.hpp"

#include "../util/util.hpp"

using namespace morda;



ColorWidget::ColorWidget(const stob::Node* chain) :
		Widget(chain)
{
	if(const stob::Node* n = getProperty(chain, "color")){
		this->color_var = n->asUint32();
	}else{
		this->color_var = 0xffffffff;
	}
}
