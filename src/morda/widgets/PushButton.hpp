/* The MIT License:

Copyright (c) 2014 Ivan Gagis

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

#include "Button.hpp"
#include "NinePatch.hpp"

namespace morda{

class PushButton :
		public Button,
		private NinePatch
{
	std::shared_ptr<ResNinePatch> imgNormal;
	std::shared_ptr<ResNinePatch> imgPressed;
	
public:

	PushButton(const PushButton&) = delete;
	PushButton& operator=(const PushButton&) = delete;
	
	PushButton(const stob::Node* chain = nullptr);
	
private:
	void OnPressedChanged()override;

	bool OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerId)override{
		return this->Button::OnMouseButton(isDown, pos, button, pointerId);
	}

	void OnHoverChanged(unsigned pointerID)override{
		this->Button::OnHoverChanged(pointerID);
	}
};

}
