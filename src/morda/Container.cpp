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







void Widget::Move(const tride::Vec2f& newPos){
	this->p = newPos;
}



void Widget::Resize(const tride::Vec2f& newDims){
	this->d = newDims;
//	TRACE(<< "Widget::Resize(): calling virtual method" << std::endl)
	this->OnResize();//call virtual method
//	TRACE(<< "Widget::Resize(): virtual method called" << std::endl)
}



void Widget::Add(ting::Ref<Widget> w){
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



void Widget::ShowModal(ting::Ref<Widget> w, bool drawWidgetsBelowModal){
	ASSERT(w)
	ASSERT(Ref<Widget>(w->parent).IsNotValid())
	ASSERT(this->modal.IsNotValid())

	this->Unhover();

	this->drawWithModal = drawWidgetsBelowModal;
	this->modal = w;
	w->parent = this;
}



void Widget::RemoveModal(){
	if(this->modal.IsNotValid())
		return;

	this->modal->parent.Reset();
	this->modal.Reset();
}



void Widget::RemoveFromParent(){
	if(Ref<Widget> p = this->parent){
		p->Remove(Ref<Widget>(this));
	}
}



void Widget::SetPriority(EPriority priority){
	//TODO:
}



//override
bool Button::OnMouseClick(const tride::Vec2f& pos, Widget::EMouseButton button, bool isDown){
//	TRACE(<< "Button::OnMouseClick(): invoked" << std::endl)
	if(button != Widget::LEFT)
		return false;

	if(isDown){
		ASSERT(!this->isPressed)
		GLWindow::Inst().SetDirty();

		this->isPressed = true;
	}else{
		if(this->isPressed){
			GLWindow::Inst().SetDirty();
			this->isPressed = false;

			this->clicked.Emit();
		}
		ASSERT(!this->isPressed)
	}
	
	return true;
}



//override
void Button::Render(const tride::Matr4f& matrix)const{
	SimpleColoringShader &s = SimpleColoringShader::Inst();
	s.UseProgram();

	s.DisableColorPointer();

	if(!this->isPressed){
		s.SetColor(Vec3f(0.5, 0.5, 0.5), 1);
//		TRACE(<< "Button::Render(): not pressed" << std::endl)
	}else{
		s.SetColor(Vec3f(1, 1, 1), 1);
//		TRACE(<< "Button::Render(): pressed" << std::endl)
	}

	Matr4f matr(matrix);
	matr.Scale(this->Dim());

	s.SetMatrix(matr);

	s.DrawQuad01();
}



//override
void Button::OnMouseOut(){
	TRACE(<< "Button::OnMouseOut(): invoked" << std::endl)
	if(this->isPressed){
		GLWindow::Inst().SetDirty();
	}
	this->isPressed = false;
}



//override
bool ToggleButton::OnMouseClick(const tride::Vec2f& pos, Widget::EMouseButton button, bool isDown){
//	TRACE(<< "ToggleButton::OnMouseClick(): invoked" << std::endl)
	if(button != Widget::LEFT)
		return false;

	if(!isDown)
		return false;

	GLWindow::Inst().SetDirty();

	this->isPressed = !this->isPressed;

	this->clicked.Emit(this->isPressed);

	return true;
}



//override
void ToggleButton::Render(const tride::Matr4f& matrix)const{
	SimpleColoringShader &s = SimpleColoringShader::Inst();
	s.UseProgram();

	s.DisableColorPointer();

	if(!this->isPressed){
		s.SetColor(Vec3f(0.5, 0, 0.5), 1);
//		TRACE(<< "ToggleButton::Render(): not pressed" << std::endl)
	}else{
		s.SetColor(Vec3f(1, 0, 1), 1);
//		TRACE(<< "ToggleButton::Render(): pressed" << std::endl)
	}

	Matr4f matr(matrix);
	matr.Scale(this->Dim());

	s.SetMatrix(matr);

	s.DrawQuad01();
}



void ToggleButton::SetPressed(bool isPressed){
	this->isPressed = isPressed;
	GLWindow::Inst().SetDirty();
}
