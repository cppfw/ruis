/* The MIT License:

Copyright (c) 2014 Ivan Gagis <igagis@gmail.com>

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

#include "Widget.hpp"


namespace morda{

class MouseProxy : virtual public Widget{
public:
	MouseProxy(const stob::Node* desc = nullptr);
	
	MouseProxy(const MouseProxy&) = delete;
	MouseProxy& operator=(const MouseProxy&) = delete;
	
	std::function<bool (Widget& widget, bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId)> onMouseButton;
	
	bool OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId)override{
		if(this->onMouseButton){
			return this->onMouseButton(*this, isDown, pos, button, pointerId);
		}
		return false;
	}
	
	std::function<bool (Widget& widget, const morda::Vec2r& pos, unsigned pointerId)> onMouseMove;
	
	bool OnMouseMove(const morda::Vec2r& pos, unsigned pointerId)override{
		if(this->onMouseMove){
			return this->onMouseMove(*this, pos, pointerId);
		}
		return false;
	}
	
	std::function<void(Widget& widget)> onHoverChanged;
	
	void OnHoverChanged()override{
		if(this->onHoverChanged){
			this->onHoverChanged(*this);
		}
	}
};

}
