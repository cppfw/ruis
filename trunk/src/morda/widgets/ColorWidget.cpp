#include "ColorWidget.hpp"

#include "../util/util.hpp"

using namespace morda;



ColorWidget::ColorWidget(const stob::Node* desc) :
		Widget(desc)
{
	if(const stob::Node* n = GetProperty(desc, "color")){
		this->color = n->AsUint32();
	}else{
		this->color = 0xffffffff;
	}
}
