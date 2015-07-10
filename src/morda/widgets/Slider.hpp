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

#include <ting/util.hpp>

#include "Widget.hpp"
#include "containers/Container.hpp"


//disable stupid warnings
#if M_COMPILER == M_COMPILER_MSVC
#	pragma warning(disable : 4250)
#endif


namespace morda{


class Slider :
		public virtual Widget,
		private Container //users do not need to know that it is a container
{
	//no copying
	Slider(const Slider&);
	Slider& operator=(const Slider&);
	
	float curFactor = 0; //Current position from 0 to 1
	float handleSizeFactor = 0; //Current handle size factor from 0 to 1
	
	unsigned GetLongIndex()const NOEXCEPT{
		return this->isVertical ? 1 : 0;
	}

	unsigned GetTransIndex()const NOEXCEPT{
		return this->isVertical ? 0 : 1;
	}
	
	Widget& handle;
	
	bool isVertical;
	
	bool isGrabbed = false;
	float clickPoint;
	
protected:
	Slider(bool isVertical, const stob::Node* chain);
	
public:
	std::function<void(Slider&)> factorChange;

	virtual ~Slider()NOEXCEPT{}
	
	float Factor()const NOEXCEPT{
		return this->curFactor;
	}
	
	void SetFactor(float newFactor);
	
private:
	void OnResize() override;
	
	morda::Vec2r onMeasure(const morda::Vec2r& quotum)const NOEXCEPT override;
	
	void Render(const morda::Matr4r& matrix) const override;
};



class VerticalSlider : public Slider{
public:
	VerticalSlider(const stob::Node* chain = nullptr) : 
			Widget(chain),
			Slider(true, chain)
	{}
	
	VerticalSlider(const VerticalSlider&) = delete;
	VerticalSlider& operator=(const VerticalSlider&) = delete;
};


class HorizontalSlider : public Slider{
public:
	HorizontalSlider(const stob::Node* chain = nullptr) : 
			Widget(chain),
			Slider(false, chain)
	{}
	
	HorizontalSlider(const HorizontalSlider&) = delete;
	HorizontalSlider& operator=(const HorizontalSlider&) = delete;
};

}//~namespace
