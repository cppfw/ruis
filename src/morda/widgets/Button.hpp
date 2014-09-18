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



class AbstractButton : public virtual Widget{
	bool isPressed = false;
	
protected:
	AbstractButton() :
			Widget(nullptr)
	{}
	
	virtual void OnPressedChanged(){}
	
	virtual void OnClicked(){}
	
	bool OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId) override;
	
	void OnHoverChanged() override;
public:	
	bool IsPressed()const NOEXCEPT{
		return this->isPressed;
	}
};



class Button : public AbstractButton{
	
	void OnClicked()override{
		if(this->onClicked){
			this->onClicked();
		}
	}

protected:
	Button() :
			Widget(nullptr)
	{}
	
public:
	std::function<void()> onClicked;
};



class AbstractToggleButton : public AbstractButton{
	bool isChecked = false;
protected:
	AbstractToggleButton(const stob::Node* desc);
	
	virtual void OnCheckedChanged(){}
public:
	
	//TODO:
	
	void SetChecked(bool checked){
		if(this->isChecked == checked){
			return;
		}
		
		this->isChecked = checked;
		this->OnCheckedChanged();
	}
	
	bool IsChecked()const NOEXCEPT{
		return this->isChecked;
	}
};



}//~namespace
