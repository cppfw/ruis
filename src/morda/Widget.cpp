#include "Widget.hpp"

#include "Container.hpp"
#include "util/util.hpp"

using namespace morda;



void Widget::ApplyProperties(const stob::Node* properties){
	if(!properties){
		this->rect.p.SetToZero();
		return;
	}
	if(const stob::Node* p = properties->GetProperty("pos")){
		this->rect.p = morda::Vec2fFromSTOB(p);
	}else{
		this->rect.p.SetToZero();
	}

	if(const stob::Node* p = properties->GetProperty("dim")){
		this->rect.d = morda::Vec2fFromSTOB(p);
	}

	if(const stob::Node* p = properties->GetProperty("name")){
		this->name = p->Value();
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
