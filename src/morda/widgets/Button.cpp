#include "Button.hpp"

#include "../App.hpp"

#include "../util/util.hpp"


using namespace morda;



//override
bool ButtonWidget::OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId){
//	TRACE(<< "AbstractButton::OnMouseButton(): isDown = " << isDown << ", button = " << button << ", pos = " << pos << std::endl)
	if(button != EMouseButton::LEFT){
		return false;
	}

	if(isDown){
		//NOTE: sometimes pos is outside of the widget. For example, when mouse is captured by this widget and
		//some other mouse button is clicked, and then the left one is clicked outside of widget,
		//then the event about left button down will still be delivered to this widget because it has captured the mouse.
		if(this->Contains(pos)){
			this->isPressed = true;
			this->OnPressedChanged();
		}
	}else{
		if(this->isPressed){
			this->isPressed = false;
			this->OnPressedChanged();
	//		TRACE(<< "AbstractButton::OnMouseButton(): emitting signal" << std::endl)
			this->OnClicked();
			return true;
		}
	}

	return true;
}



//override
void ButtonWidget::OnHoverChanged(){
//	TRACE(<< "AbstractButton::OnHoverChanged(): enter" << std::endl)
	
	if(!this->IsHovered()){
		this->isPressed = false;
		this->OnPressedChanged();
	}
}



ToggleButton::ToggleButton(const stob::Node* chain) :
		Widget(chain)
{
	if(const stob::Node* n = GetProperty(chain, "checked")){
		this->isChecked = n->AsBool();
	}else{
		this->isChecked = false;
	}
}

