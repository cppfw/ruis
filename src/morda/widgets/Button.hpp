/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include <functional>

#include "../widgets/Widget.hpp"



namespace morda{



class Button : public virtual Widget{
	bool isPressed_var = false;
	
protected:
	Button() :
			Widget(nullptr)
	{}
	
	virtual void onPressedChanged(){
		if(this->pressedChanged){
			this->pressedChanged(*this);
		}
	}
	
	bool onMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId) override;
	
	void onHoverChanged(unsigned pointerID) override;
public:	
	bool isPressed()const noexcept{
		return this->isPressed_var;
	}
	
	std::function<void(Button&)> pressedChanged;
};



class PushButton : public Button{
	bool currentlyPressed = false;

protected:
	void onPressedChanged()override;
	
	virtual void onClicked(){
		if(this->clicked){
			this->clicked(*this);
		}
	}
	
	PushButton() :
			Widget(nullptr)
	{}
	
public:
	std::function<void(PushButton&)> clicked;
};



class ToggleButton : public PushButton{
	bool isChecked_var;
	
protected:
	void onClicked()override{
		this->toggle();
	}
	
	ToggleButton(const stob::Node* chain);
	
	virtual void onCheckedChanged(){
		if(this->checkedChanged){
			this->checkedChanged(*this, this->isChecked());
		}
	}
public:

	void setChecked(bool checked){
		if(this->isChecked_var == checked){
			return;
		}
		
		this->isChecked_var = checked;
		this->onCheckedChanged();
	}
	
	void toggle(){
		this->setChecked(!this->isChecked());
	}
	
	bool isChecked()const noexcept{
		return this->isChecked_var;
	}
	
	std::function<void(ToggleButton&, bool)> checkedChanged;
};



}//~namespace
