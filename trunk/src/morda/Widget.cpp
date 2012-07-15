#include "Widget.hpp"

#include "Container.hpp"


using namespace morda;



void Widget::RemoveFromParent(){
	if(ting::Ref<Container> p = this->parent){
		bool res = p->Remove(ting::Ref<Widget>(this));
		ASSERT(res)
	}
}
