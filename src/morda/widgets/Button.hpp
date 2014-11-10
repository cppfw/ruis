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
	bool isPressed = false;
	
protected:
	Button() :
			Widget(nullptr)
	{}
	
	virtual void OnPressedChanged(){}
	
	virtual void OnClicked(){}
	
	bool OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId) override;
	
	void OnHoverChanged(unsigned pointerID) override;
public:	
	bool IsPressed()const NOEXCEPT{
		return this->isPressed;
	}
};



class PushButton : public Button{
	
	void OnClicked()override final{
		if(this->onClicked){
			this->onClicked();
		}
	}

protected:
	PushButton() :
			Widget(nullptr)
	{}
	
public:
	std::function<void()> onClicked;
};



class ToggleButton : public Button{
	bool isChecked;
	
	void OnClicked()override final{
		this->Toggle();
	}
	
protected:
	ToggleButton(const stob::Node* chain);
	
	virtual void OnCheckedChanged(){
		if(this->onCheckedChanged){
			this->onCheckedChanged(this->IsChecked());
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
	
	bool IsChecked()const NOEXCEPT{
		return this->isChecked;
	}
	
	std::function<void(bool)> onCheckedChanged;
};



}//~namespace
