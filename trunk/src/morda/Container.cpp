#include "Container.hpp"

#include "App.hpp"

#include "util/util.hpp"



using namespace morda;



void Container::ApplyDescription(const stob::Node& description){
	if(const stob::Node* n = description.GetProperty("layout")){
		this->SetLayout(morda::App::Inst().Inflater().CreateLayout(*n));
	}
	
	for(const stob::Node* n = description.Child(); n; n = n->Next()){
		if(!n->IsCapital()){
			continue;//skip properties
		}
		
		this->Add(morda::App::Inst().Inflater().Inflate(*n));
	}
}



//override
void Container::Render(const morda::Matr4f& matrix)const{
#ifdef M_MORDA_RENDER_WIDGET_BORDERS
	this->Widget::Render(matrix);
#endif
	
	for(ting::Ref<Widget>* c = &this->childrenHead; *c; c = &(*c)->Next()){
		if((*c)->IsHidden()){
			continue;
		}
		
		morda::Matr4f matr(matrix);
		matr.Translate((*c)->Rect().p);

		(*c)->Render(matr);
	}
}



//override
bool Container::OnMouseButtonDown(const morda::Vec2f& pos, EMouseButton button, unsigned pointerId){
//	TRACE(<< "Container::OnMouseButtonDown(): enter, button = " << button << ", pos = " << pos << std::endl)
	//Copy children list to iterate through it later, because the original list of children
	//may change during iterating.
	T_ChildList childs;
	childs.reserve(this->children.size());
	for(T_ChildList::iterator i = this->children.begin(); i != this->children.end(); ++i){
		childs.push_back(*i);
	}
	
	//call children in reverse order
	for(T_ChildList::reverse_iterator i = childs.rbegin(); i != childs.rend(); ++i){
		if((*i)->isHidden || (*i)->isDisabled){
			continue;
		}
		
		if(!(*i)->Rect().Overlaps(pos)){
			continue;
		}
		
		//Sometimes mouse click event comes without prior mouse move,
		//but, since we get mouse click, then the widget is hovered.
		if(!(*i)->IsHovered()){
			(*i)->isHovered = true;
			(*i)->OnMouseIn();
		}
		
		if((*i)->OnMouseButtonDown((pos - (*i)->Rect().p), button, pointerId)){
			return true;
		}
	}
	return false;
}



//override
bool Container::OnMouseButtonUp(const morda::Vec2f& pos, EMouseButton button, unsigned pointerId){
//	TRACE(<< "Container::OnMouseButtonUp(): enter, button = " << button << ", pos = " << pos << std::endl)
	
	//Copy children list to iterate through it later, because the original list of children
	//may change during iterating.
	T_ChildList childs;
	childs.reserve(this->children.size());
	for(T_ChildList::iterator i = this->children.begin(); i != this->children.end(); ++i){
		childs.push_back(*i);
	}
	
	//call children in reverse order
	for(T_ChildList::reverse_iterator i = childs.rbegin(); i != childs.rend(); ++i){
		if((*i)->isHidden || (*i)->isDisabled){
			continue;
		}
		
		if(!(*i)->Rect().Overlaps(pos)){
			continue;
		}
		
		//Sometimes mouse click event comes without prior mouse move,
		//but, since we get mouse click, then the widget is hovered.
		if(!(*i)->IsHovered()){
			(*i)->isHovered = true;
			(*i)->OnMouseIn();
		}
		
		if((*i)->OnMouseButtonUp(pos - (*i)->Rect().p, button, pointerId)){
			return true;
		}
	}
	return false;
}



//override
bool Container::OnMouseMove(const morda::Vec2f& pos, unsigned pointerId){
	//Copy children list to iterate through it later, because the original list of children
	//may change during iterating.
	T_ChildList childs;
	childs.reserve(this->children.size());
	for(T_ChildList::iterator i = this->children.begin(); i != this->children.end(); ++i){
		childs.push_back(*i);
	}
	
	//call children in reverse order
	for(T_ChildList::reverse_iterator i = childs.rbegin(); i != childs.rend(); ++i){
		if((*i)->isHidden){
			ASSERT(!(*i)->IsHovered())
			continue;
		}
		
		if(!(*i)->Rect().Overlaps(pos)){
			if((*i)->IsHovered()){
				(*i)->isHovered = false;
				(*i)->OnMouseOut();
			}
			continue;
		}
		
		if(!(*i)->IsHovered()){
			(*i)->isHovered = true;
			(*i)->OnMouseIn();
		}
		
		if((*i)->OnMouseMove(pos - (*i)->Rect().p, pointerId)){//consumed mouse move event
			//un-hover rest of the children
			for(++i; i != childs.rend(); ++i){
				if((*i)->IsHovered()){
					(*i)->isHovered = false;
					(*i)->OnMouseOut();
				}
			}
			return true;
		}		
	}
	return false;
}



void Container::OnMouseOut(){
	//un-hover all the children
	for(T_ChildList::iterator i = this->children.begin(); i != this->children.end(); ++i){
		if((*i)->IsHovered()){
			(*i)->isHovered = false;
			(*i)->OnMouseOut();
		}
	}
}



void Container::OnResize(){
//	TRACE(<< "Container::OnResize(): invoked" << std::endl)
	if(this->layout){
		this->layout->ArrangeWidgets(*this);
	}
}



//override
morda::Vec2f Container::ComputeMinimalDimensions()const throw(){
	if(this->layout){
		return this->layout->ComputeMinimalDimensions(*this);
	}
	return this->Widget::ComputeMinimalDimensions();
}



void Container::Add(const ting::Ref<Widget>& w){
	ASSERT_INFO(w, "Widget::Add(): widget pointer is 0")
	if(w->parent.GetRef().IsValid()){
		throw morda::Exc("Container::Add(): cannot add widget, it is already added to some container");
	}
	ASSERT(w->next.IsNotValid())
	ASSERT(w->prev.IsNotValid())
	
	if(!this->childrenHead){
		ASSERT(!this->childrenTail)
		this->childrenHead = w;
		this->childrenTail = w;
	}else{
		this->childrenTail->next = w;
		w->prev = this->childrenTail;
		this->childrenTail = w;
	}
	
	w->parent = this;

	w->RelayoutNeeded();//will call to this->RelayoutNeeded() also since parent is already set
	
	ASSERT(!w->IsHovered())
}



void Container::Remove(const ting::Ref<Widget>& w){
	ASSERT(w.IsValid())
	ASSERT(w->parent.GetRef() == this)

	if(w->prev){
		w->prev->next = w->next;
	}else{
		this->childrenHead = w->next;
	}
	if(w->next){
		w->next->prev = w->prev;
	}else{
		this->childrenTail = w->prev;
	}
	
	w->parent.Reset();
	if(w->IsHovered()){
		w->isHovered = false;
		w->OnMouseOut();
	}
	this->RelayoutNeeded();
}
