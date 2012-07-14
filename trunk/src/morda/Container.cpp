#include <tride/Vector3.hpp>

#include "Widget.hpp"
#include "Shader.hpp"
#include "GLWindow.hpp"



using namespace morda;



void Widget::RenderWithChildren(const tride::Matr4f& matrix)const{
	if(this->isHidden)
		return;

	tride::Matr4f m;
	m.Identity();
	m.Translate(this->p);
	m.LeftMultMatrix(matrix);

	if(!this->modal || this->drawWithModal){
		this->Render(m);

		for(T_ChildConstIter i = this->children.begin(); i != this->children.end(); ++i){
			(*i)->RenderWithChildren(m);
		}
	}

	if(this->modal){
		this->modal->RenderWithChildren(m);
	}
}



bool Widget::OnMouseClickInternal(const tride::Vec2f& pos, EMouseButton button, bool isDown){
	if(this->isHidden || this->isDisabled)
		return false;
	
	if(!this->IsInWidgetRect(pos))
		return false;

	tride::Vec2f localPos = pos - this->p;

	//If there is a modal widget, then pass input only to that modal widget
	if(Ref<Widget> modalWidget = this->modal){
		modalWidget->OnMouseClickInternal(localPos, button, isDown);
		return true;//return always true, since we have modal widget
	}

	//Sometimes mouse click event comes without prior mouse move,
	//but, since we get mouse click, then the widget is hovered.
	if(!this->IsHovered()){
		this->isHovered = true;
		ASSERT(this->IsHovered())
		this->OnMouseIn();
	}

	//Copy children list to iterate through it later, because the original list of childs
	//may change during iterating.
	T_ChildList childs;
	childs.reserve(this->children.size());
	for(T_ChildIter i = this->children.begin(); i != this->children.end(); ++i){
		childs.push_back(*i);
	}

	if(childs.size() > 0){
		//call children in reverse order
		for(T_ChildList::reverse_iterator i = childs.rbegin(); i != childs.rend(); ++i){
			if((*i)->OnMouseClickInternal(localPos, button, isDown)){
				return true;
			}
		}
	}

	return this->OnMouseClick(localPos, button, isDown);
}



bool Widget::OnMouseMoveInternal(const tride::Vec2f& oldPos, const tride::Vec2f& newPos, const tride::Vec2f& dpos){
//	TRACE(<< "Widget::OnMouseMoveInternal(): enter, isHidden = " << this->isHidden << std::endl)
	if(this->isHidden){
		ASSERT(!this->IsHovered())
		return false;
	}

	if(this->isDisabled){
		return false;
	}

	bool newPosInside = this->IsInWidgetRect(newPos);

	if(!newPosInside){
		if(this->IsHovered()){
			this->Unhover();
		}
		return false;
	}

	tride::Vec2f localNewPos = newPos - this->p;
	tride::Vec2f localOldPos = oldPos - this->p;


	//If there is a modal widget, then pass input only to that modal widget
	if(Ref<Widget> modalWidget = this->modal){
		bool res = modalWidget->OnMouseMoveInternal(localOldPos, localNewPos, dpos);
		return res;
	}

	ASSERT(newPosInside)

	if(!this->IsHovered()){
		this->isHovered = true;
		ASSERT(this->IsHovered())
		this->OnMouseIn();
	}

	//Copy children list to iterate through it later, because the original list of childs
	//may change during iterating.
	T_ChildList childs;
	childs.reserve(this->children.size());
	for(T_ChildIter i = this->children.begin(); i != this->children.end(); ++i){
		childs.push_back(*i);
	}

	if(childs.size() > 0){
		//call children in reverse order
		for(T_ChildList::reverse_iterator i = childs.rbegin(); i != childs.rend(); ++i){
			bool res = (*i)->OnMouseMoveInternal(localOldPos, localNewPos, dpos);
			if(res){//consumed mouse move event
				//unhover rest of the children
				for(++i; i != childs.rend(); ++i){
					(*i)->Unhover();
				}
				return true;
			}
		}
	}

	return this->OnMouseMove(localOldPos, localNewPos, dpos);
}



void Container::Add(ting::Ref<Widget> w){
	ASSERT_INFO(w, "Widget::Add(): widget pointer is 0")
	ASSERT(Ref<Widget>(w->parent).IsNotValid())
	this->children.push_back(w);
	w->parent = this;

	ASSERT(!w->IsHovered())
}



bool Container::Remove(ting::Ref<Widget> w){
	ASSERT(w.IsValid())

	if(this->modal == w){
		this->modal.Reset();
		return true;
	}

	for(T_ChildIter i = this->children.begin(); i != this->children.end(); ++i){
		if(w == (*i)){
			this->children.erase(i);
			w->parent.Reset();
			w->isHovered = false;
			w->OnMouseOut();
			return true;
		}
	}
	return false;
}



void Widget::RemoveFromParent(){
	if(Ref<Widget> p = this->parent){
		p->Remove(Ref<Widget>(this));
	}
}


