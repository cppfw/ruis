/* The MIT License:

Copyright (c) 2014-2015 Ivan Gagis <igagis@gmail.com>

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

#include "Container.hpp"

#include <functional>


namespace morda{



class ScrollContainer : public Container{
	//offset from top left corner
	Vec2r curScrollPos = Vec2r(0);
	
	//cached effectiveDim
	Vec2r effectiveDim;
	
	//cached scroll factor
	Vec2r curScrollFactor;
	
public:
	ScrollContainer(const stob::Node* chain = nullptr);
	
	ScrollContainer(const ScrollContainer&) = delete;
	ScrollContainer& operator=(const ScrollContainer&) = delete;
	

	bool onMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerID)override;
	
	bool onMouseMove(const morda::Vec2r& pos, unsigned pointerID)override;
	
	void render(const morda::Matr4r& matrix) const override;

	morda::Vec2r measure(const morda::Vec2r& quotum) const override{
		return this->Widget::measure(quotum);
	}

	void layOut()override;

	void onChildrenListChanged()override;
	
	const Vec2r& scrollPos()const{
		return this->curScrollPos;
	}
	
	
	void setScrollPos(const Vec2r& newScrollPos);
	
	void setScrollPosAsFactor(const Vec2r& factor);
	
	const Vec2r& scrollFactor()const{
		return this->curScrollFactor;
	}
	
private:
	void UpdateEffectiveDim();
	
	void UpdateScrollFactor();

	void ClampScrollPos();
	
	void arrangeWidgets();
};



}
