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

#include "DimContainer.hpp"


namespace morda{



class ScrollContainer :
		virtual public Widget,
		private DimContainer
{
	Vec2r scrollPos = Vec2r(0);
	
	Vec2b reverse;
	
public:
	ScrollContainer(const stob::Node* chain = nullptr);
	
	ScrollContainer(const ScrollContainer&) = delete;
	ScrollContainer& operator=(const ScrollContainer&) = delete;
	

	bool OnMouseButton(bool isDown, const morda::Vec2r& pos, EMouseButton button, unsigned pointerID)override;
	
	bool OnMouseMove(const morda::Vec2r& pos, unsigned pointerID)override;
	
	void Render(const morda::Matr4r& matrix) const override;

	morda::Vec2r ComputeMinDim() const override;

	
	
	const Vec2r& ScrollPos()const{
		return this->scrollPos;
	}
	
	void SetScrollPos(const Vec2r& newScrollPos);
	
	void SetScrollFactor(const Vec2r& factor);
	
	Vec2r ScrollFactor()const;
	
	
private:
	
	Vec2r ComputeEffectiveDim()const{
		return this->DimContainer::ComputeMinDim() - this->Rect().d;
	}

};



}
