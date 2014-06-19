#include "Container.hpp"

#include "../App.hpp"

#include "../util/util.hpp"



using namespace morda;



Container::Container(const stob::Node& description) :
		Widget(description)
{
	for(const stob::Node* n = description.Child(); n; n = n->Next()){
		if(n->IsProperty()){
			continue;//skip properties
		}
		
		this->Add(morda::App::Inst().inflater().Inflate(*n));
	}
}



//override
void Container::Render(const morda::Matr4f& matrix)const{
	for(Widget::T_ChildrenList::const_iterator i = this->Children().begin(); i != this->Children().end(); ++i){
		if((*i)->IsHidden()){
			continue;
		}
		
		morda::Matr4f matr(matrix);
		matr.Translate((*i)->Rect().p);

		(*i)->RenderInternal(matr);
	}
}



//override
bool Container::OnMouseButton(bool isDown, const morda::Vec2f& pos, EMouseButton button, unsigned pointerId){
//	TRACE(<< "Container::OnMouseButton(): isDown = " << isDown << ", button = " << button << ", pos = " << pos << std::endl)
	
	//check if mouse captured
	{
		T_MouseCaptureMap::iterator i = this->mouseCaptureMap.find(pointerId);
		if(i != this->mouseCaptureMap.end()){
			if(ting::Ref<Widget> w = i->second.first){
				w->SetHovered(w->Rect().Overlaps(pos));
				w->OnMouseButton(isDown, pos - w->Rect().p, button, pointerId);
				
				unsigned& n = i->second.second;
				if(isDown){
					++n;
				}else{
					--n;
				}
				if(n == 0){
					this->mouseCaptureMap.erase(i);
				}
				return true;//doesn't matter what to return
			}else{
				this->mouseCaptureMap.erase(i);
			}
		}
	}
	
	//original list of children may change during iterating.
	//TODO: see same comment in OnMouseMove
	
	//call children in reverse order
	for(Widget::T_ChildrenList::const_reverse_iterator i = this->Children().rbegin(); i != this->Children().rend(); ++i){
		if((*i)->IsHidden() || (*i)->IsDisabled()){
			continue;
		}
		
		if(!(*i)->Rect().Overlaps(pos)){
			continue;
		}
		
		//Sometimes mouse click event comes without prior mouse move,
		//but, since we get mouse click, then the widget was hovered before the click.
		(*i)->SetHovered(true);
		if((*i)->OnMouseButton(isDown, pos - (*i)->Rect().p, button, pointerId)){
			ASSERT(this->mouseCaptureMap.find(pointerId) == this->mouseCaptureMap.end())
			
			if(isDown){//in theory, it can be button up event here, if some widget which captured mouse was removed from its parent
				this->mouseCaptureMap.insert(std::make_pair(pointerId, std::make_pair((*i).GetWeakRef(), 1)));
			}
			
			return true;
		}
	}
	return false;
}



//override
bool Container::OnMouseMove(const morda::Vec2f& pos, unsigned pointerId){
//	TRACE(<< "Container::OnMouseMove(): pos = " << pos << std::endl)
	
	//check if mouse captured
	{
		T_MouseCaptureMap::iterator i = this->mouseCaptureMap.find(pointerId);
		if(i != this->mouseCaptureMap.end()){
			if(ting::Ref<Widget> w = i->second.first){
				w->OnMouseMove(pos - w->Rect().p, pointerId);
		
				//set hovered goes after move notification because position of widget could change
				//during handling the notification, so need to check after that for hovering
				w->SetHovered(w->Rect().Overlaps(pos));

				return true;//doesn't matter what to return
			}else{
				this->mouseCaptureMap.erase(i);
			}
		}
	}
	
	//original list of children may change during iterating.
	//TODO: forbid adding deleting widgets during this function call, add deferred add delete functions
	
	//call children in reverse order
	for(Widget::T_ChildrenList::const_reverse_iterator i = this->Children().rbegin(); i != this->Children().rend(); ++i){
		if((*i)->isHidden){
			ASSERT(!(*i)->IsHovered())
			continue;
		}
		
		if(!(*i)->Rect().Overlaps(pos)){
			(*i)->SetHovered(false);
			continue;
		}
		
		(*i)->SetHovered(true);
		
		if((*i)->OnMouseMove(pos - (*i)->Rect().p, pointerId)){//consumed mouse move event
			//un-hover rest of the children
			for(++i; i != this->Children().rend(); ++i){
				(*i)->SetHovered(false);
			}
			return true;
		}		
	}
	return false;
}



void Container::OnHoverChanged(){
	//TODO: if some child removed during iterating?
	
	if(this->IsHovered()){
		return;
	}
	
	//un-hover all the children if container became un-hovered
	for(Widget::T_ChildrenList::const_iterator i = this->Children().begin(); i != this->Children().end(); ++i){
		(*i)->SetHovered(false);
	}
}



void Container::OnResize(){
//	TRACE(<< "Container::OnResize(): invoked" << std::endl)
	for(Widget::T_ChildrenList::const_iterator i = this->Children().begin(); i != this->Children().end(); ++i){
		if((*i)->NeedsRelayout()){
			(*i)->Resize((*i)->Rect().d);
		}
	}
}



void Container::Add(const ting::Ref<Widget>& w){
	ASSERT_INFO(w, "Container::Add(): widget pointer is 0")
	if(w->parent.GetRef().IsValid()){
		throw morda::Exc("Container::Add(): cannot add widget, it is already added to some container");
	}

	this->children.push_back(w);
	w->parentIter = --this->children.end();
	
	w->parent = this;

	this->SetRelayoutNeeded();
	
	ASSERT(!w->IsHovered())
}



void Container::Remove(const ting::Ref<Widget>& w){
	ASSERT(w.IsValid())
	ASSERT(w->parent.GetRef() == ting::Ref<Container>(this))

	if(w->parent.GetRef().operator->() != this){
		throw morda::Exc("Container::Remove(): widget is not added to this container");
	}
	
	this->children.erase(w->parentIter);
	
	w->parent.Reset();
	w->SetHovered(false);
	
	this->SetRelayoutNeeded();
}



//override
ting::Ref<Widget> Container::FindChildByName(const std::string& name)throw(){
//	TRACE_AND_LOG(<< "Container::FindChildByName(): enter" << std::endl)
	for(Widget::T_ChildrenList::const_iterator i = this->Children().begin(); i != this->Children().end(); ++i){
//		TRACE_AND_LOG(<< "(*c)->Name() = " << ((*c)->Name()) << std::endl)
		if((*i)->Name() == name){
			return *i;
		}
		
		if(ting::Ref<Widget> r = (*i)->FindChildByName(name)){
			return r;
		}
	}
	return 0;
}
