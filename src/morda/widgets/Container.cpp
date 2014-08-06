#include "Container.hpp"

#include "../App.hpp"

#include "../util/util.hpp"



using namespace morda;



namespace{
class BlockedFlagGuard{
	bool& blocked;
public:
	BlockedFlagGuard(bool& blocked) :
			blocked(blocked)
	{
		this->blocked = true;
	}
	
	~BlockedFlagGuard()noexcept{
		this->blocked = false;
	}
};
}



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
	
	BlockedFlagGuard blockedFlagGuard(this->isBlocked);
	
	//check if mouse captured
	{
		T_MouseCaptureMap::iterator i = this->mouseCaptureMap.find(pointerId);
		if(i != this->mouseCaptureMap.end()){
			if(auto w = i->second.first.lock()){
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
				this->mouseCaptureMap.insert(std::make_pair(pointerId, std::make_pair(std::weak_ptr<Widget>(*i), 1)));
			}
			
			return true;
		}
	}
	
	return this->Widget::OnMouseButton(isDown, pos, button, pointerId);
}



//override
bool Container::OnMouseMove(const morda::Vec2f& pos, unsigned pointerId){
//	TRACE(<< "Container::OnMouseMove(): pos = " << pos << std::endl)
	
	BlockedFlagGuard blockedFlagGuard(this->isBlocked);
	
	//check if mouse captured
	{
		T_MouseCaptureMap::iterator i = this->mouseCaptureMap.find(pointerId);
		if(i != this->mouseCaptureMap.end()){
			if(auto w = i->second.first.lock()){
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
	
	return this->Widget::OnMouseMove(pos, pointerId);
}



void Container::OnHoverChanged(){
	//TODO: if some child removed during iterating?
	
	if(this->IsHovered()){
		return;
	}
	
	//un-hover all the children if container became un-hovered
	BlockedFlagGuard blockedFlagGuard(this->isBlocked);
	for(Widget::T_ChildrenList::const_iterator i = this->Children().begin(); i != this->Children().end(); ++i){
		(*i)->SetHovered(false);
	}
}



void Container::OnResize(){
//	TRACE(<< "Container::OnResize(): invoked" << std::endl)
	BlockedFlagGuard blockedFlagGuard(this->isBlocked);
	for(Widget::T_ChildrenList::const_iterator i = this->Children().begin(); i != this->Children().end(); ++i){
		if((*i)->NeedsRelayout()){
			(*i)->OnResize();
		}
	}
}



void Container::Add(const std::shared_ptr<Widget>& w){
	ASSERT_INFO(w, "Container::Add(): widget pointer is 0")
	if(w->Parent()){
		throw morda::Exc("Container::Add(): cannot add widget, it is already added to some container");
	}

	this->children.push_back(w);
	w->parentIter = --this->children.end();
	
	w->parent = this;

	this->SetRelayoutNeeded();
	
	ASSERT(!w->IsHovered())
}



void Container::Remove(Widget& w){
	if(w.parent != this){
		throw morda::Exc("Container::Remove(): widget is not added to this container");
	}
	
	if(this->isBlocked){
		throw morda::Exc("Container::Remove(): cannot remove child while iterating through children, try deferred removing.");
	}
	
	ASSERT(w.parent == this)
	
	this->children.erase(w.parentIter);
	
	w.parent = 0;
	w.SetHovered(false);
	
	this->SetRelayoutNeeded();
}



//override
std::shared_ptr<Widget> Container::FindChildByName(const std::string& name)noexcept{
//	TRACE_AND_LOG(<< "Container::FindChildByName(): enter" << std::endl)
	for(Widget::T_ChildrenList::const_iterator i = this->Children().begin(); i != this->Children().end(); ++i){
//		TRACE_AND_LOG(<< "(*c)->Name() = " << ((*c)->Name()) << std::endl)
		if((*i)->Name() == name){
			return *i;
		}
		
		if(auto r = (*i)->FindChildByName(name)){
			return r;
		}
	}
	return 0;
}
