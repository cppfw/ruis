#include <tride/Vector3.hpp>

#include "Container.hpp"



using namespace morda;



//override
void Container::Render(const tride::Matr4f& matrix)const{
	for(T_ChildList::const_iterator i = this->children.begin(); i != this->children.end(); ++i){
		if((*i)->IsHidden()){
			continue;
		}
		
		tride::Matr4f matr(matrix);
		matr.Translate((*i)->Pos());

		(*i)->Render(matr);
	}
}



//override
bool Container::OnMouseButtonDown(const tride::Vec2f& pos, EMouseButton button, unsigned pointerId){
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
		
		if(!(*i)->IsInWidgetRect(pos)){
			continue;
		}
		
		//Sometimes mouse click event comes without prior mouse move,
		//but, since we get mouse click, then the widget is hovered.
		if(!(*i)->IsHovered()){
			(*i)->isHovered = true;
			(*i)->OnMouseIn();
		}

		tride::Vec2f localPos = pos - (*i)->p;
		
		if((*i)->OnMouseButtonDown(localPos, button, pointerId)){
			return true;
		}
	}
	return false;
}



//override
bool Container::OnMouseButtonUp(const tride::Vec2f& pos, EMouseButton button, unsigned pointerId){
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
		
		if(!(*i)->IsInWidgetRect(pos)){
			continue;
		}
		
		//Sometimes mouse click event comes without prior mouse move,
		//but, since we get mouse click, then the widget is hovered.
		if(!(*i)->IsHovered()){
			(*i)->isHovered = true;
			(*i)->OnMouseIn();
		}

		tride::Vec2f localPos = pos - (*i)->p;
		
		if((*i)->OnMouseButtonUp(localPos, button, pointerId)){
			return true;
		}
	}
	return false;
}



//override
bool Container::OnMouseMove(const tride::Vec2f& pos, unsigned pointerId){
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
		
		if(!(*i)->IsInWidgetRect(pos)){
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
		
		if((*i)->OnMouseMove(pos, pointerId)){//consumed mouse move event
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



void Container::OnResize(){
	//TODO:
}



void Container::Add(const ting::Ref<Widget>& w){
	ASSERT_INFO(w, "Widget::Add(): widget pointer is 0")
	ASSERT(ting::Ref<Widget>(w->parent).IsNotValid())
	this->children.push_back(w);
	w->parent = this;

	ASSERT(!w->IsHovered())
}



bool Container::Remove(const ting::Ref<Widget>& w){
	ASSERT(w.IsValid())

	for(T_ChildList::iterator i = this->children.begin(); i != this->children.end(); ++i){
		if(w == (*i)){
			this->children.erase(i);
			w->parent.Reset();
			if(w->IsHovered()){
				w->isHovered = false;
				w->OnMouseOut();
			}
			return true;
		}
	}
	return false;
}
