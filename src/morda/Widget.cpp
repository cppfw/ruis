#include "Widget.hpp"

#include "Container.hpp"


using namespace morda;



Widget::Widget(ting::Ptr<stob::Node> properties) :
		properties(properties)
{
//	if(const stob::Node* p = this->Properties()->GetProperty("pos")){
//		//TODO:
//
//	}
//	
//	if(const stob::Node* p = this->Properties()->GetProperty("dim")){
//		//TODO:
//
//	}
	
}



void Widget::RemoveFromParent(){
	if(ting::Ref<Container> p = this->parent){
#ifdef DEBUG
		bool res =
#endif
		p->Remove(ting::Ref<Widget>(this));
		ASSERT(res)
	}
}
