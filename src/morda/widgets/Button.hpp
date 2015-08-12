/* The MIT License:

Copyright (c) 2012-2014 Ivan Gagis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

// Home page: http://morda.googlecode.com

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
	
	virtual void OnPressedChanged(){
		if(this->pressedChanged){
			this->pressedChanged(*this);
		}
	}
	
	bool OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId) override;
	
	void OnHoverChanged(unsigned pointerID) override;
public:	
	bool isPressed()const noexcept{
		return this->isPressed_var;
	}
	
	std::function<void(Button&)> pressedChanged;
};



class PushButton : public Button{
	bool currentlyPressed = false;

protected:
	void OnPressedChanged()override;
	
	virtual void OnClicked(){
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
	bool isChecked;
	
protected:
	void OnClicked()override{
		this->Toggle();
	}
	
	ToggleButton(const stob::Node* chain);
	
	virtual void OnCheckedChanged(){
		if(this->checkedChanged){
			this->checkedChanged(*this, this->IsChecked());
		}
	}
public:

	void SetChecked(bool checked){
		if(this->isChecked == checked){
			return;
		}
		
		this->isChecked = checked;
		this->OnCheckedChanged();
	}
	
	void Toggle(){
		this->SetChecked(!this->IsChecked());
	}
	
	bool IsChecked()const noexcept{
		return this->isChecked;
	}
	
	std::function<void(ToggleButton&, bool)> checkedChanged;
};



}//~namespace
