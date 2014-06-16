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
	for(const ting::Ref<Widget>* c = &this->childrenHead; *c; c = &(*c)->Next()){
		if((*c)->IsHidden()){
			continue;
		}
		
		morda::Matr4f matr(matrix);
		matr.Translate((*c)->Rect().p);

		(*c)->RenderInternal(matr);
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
	//TODO:
	
	//call children in reverse order
	for(const ting::Ref<Widget>* c = &this->childrenTail; *c; c = &(*c)->Prev()){
		if((*c)->IsHidden() || (*c)->IsDisabled()){
			continue;
		}
		
		if(!(*c)->Rect().Overlaps(pos)){
			continue;
		}
		
		//Sometimes mouse click event comes without prior mouse move,
		//but, since we get mouse click, then the widget was hovered before the click.
		(*c)->SetHovered(true);
		if((*c)->OnMouseButton(isDown, pos - (*c)->Rect().p, button, pointerId)){
			ASSERT(this->mouseCaptureMap.find(pointerId) == this->mouseCaptureMap.end())
			
			if(isDown){//in theory, it can be button up event here, if some widget which captured mouse was removed from its parent
				this->mouseCaptureMap.insert(std::make_pair(pointerId, std::make_pair((*c).GetWeakRef(), 1)));
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
	//TODO:
	
	//call children in reverse order
	for(const ting::Ref<Widget>* c = &this->childrenTail; *c; c = &(*c)->Prev()){
		if((*c)->isHidden){
			ASSERT(!(*c)->IsHovered())
			continue;
		}
		
		if(!(*c)->Rect().Overlaps(pos)){
			(*c)->SetHovered(false);
			continue;
		}
		
		(*c)->SetHovered(true);
		
		if((*c)->OnMouseMove(pos - (*c)->Rect().p, pointerId)){//consumed mouse move event
			//un-hover rest of the children
			for(c = &(*c)->Prev(); *c; c = &(*c)->Prev()){
				(*c)->SetHovered(false);
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
	for(const ting::Ref<Widget>* c = &this->childrenHead; *c; c = &(*c)->Next()){
		(*c)->SetHovered(false);
	}
}



void Container::OnResize(){
//	TRACE(<< "Container::OnResize(): invoked" << std::endl)
	for(const ting::Ref<Widget>* c = &this->Children(); *c; c = &(*c)->Next()){
		if((*c)->NeedsRelayout()){
			(*c)->Resize((*c)->Rect().d);
		}
	}
}



void Container::Add(const ting::Ref<Widget>& w){
	ASSERT_INFO(w, "Container::Add(): widget pointer is 0")
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
	
	++this->numChildren;
	
	w->parent = this;

	this->SetRelayoutNeeded();
	
	ASSERT(!w->IsHovered())
}



void Container::Remove(const ting::Ref<Widget>& w){
	ASSERT(w.IsValid())
	ASSERT(w->parent.GetRef() == ting::Ref<Container>(this))

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
	
	--this->numChildren;
	
	w->parent.Reset();
	w->SetHovered(false);
	
	this->SetRelayoutNeeded();
}



//override
ting::Ref<Widget> Container::FindChildByName(const std::string& name)throw(){
//	TRACE_AND_LOG(<< "Container::FindChildByName(): enter" << std::endl)
	for(const ting::Ref<Widget>* c = &this->Children(); *c; c = &(*c)->Next()){
//		TRACE_AND_LOG(<< "(*c)->Name() = " << ((*c)->Name()) << std::endl)
		if((*c)->Name() == name){
			return *c;
		}
		
		if(ting::Ref<Widget> r = (*c)->FindChildByName(name)){
			return r;
		}
	}
	return 0;
}
