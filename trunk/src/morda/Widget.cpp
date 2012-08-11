#include "Widget.hpp"

#include "Container.hpp"
#include "util/util.hpp"

using namespace morda;



Widget::Widget(ting::Ptr<stob::Node> properties) :
		properties(properties)
{
	if(this->properties){
		if(const stob::Node* p = this->properties->GetProperty("pos")){
			this->rect.p = morda::Vec2fFromSTOB(p);
		}

		if(const stob::Node* p = this->properties->GetProperty("dim")){
			this->rect.d = morda::Vec2fFromSTOB(p);
		}
		
		if(const stob::Node* p = this->properties->GetProperty("name")){
			this->name = p->Value();
		}
	}
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
