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

#include <ting/util.hpp>
#include <ting/Signal.hpp>

#include "Widget.hpp"
#include "Container.hpp"
#include "LinearWidget.hpp"


//diable stupid warnings
#if M_COMPILER == M_COMPILER_MSVC
#	pragma warning(disable : 4250)
#endif


namespace morda{


class Slider :
		public virtual Widget,
		private Container, //users do not need to know that it is a container
		public LinearWidget
{
	//no copying
	Slider(const Slider&);
	Slider& operator=(const Slider&);
	
	float curFactor = 0; //Current position from 0 to 1
	float handleSizeFactor = 0; //Current handle size factor from 0 to 1
	
	class SliderHandle : public Widget{
		Slider& slider;

		bool isGrabbed = false;
		float clickPoint;

	public:
		SliderHandle(Slider& slider) :
				Widget(0),
				slider(slider)
		{}
	public:

	private:
		void Render(const morda::Matr4f& matrix)const override;

		bool OnMouseButton(bool isDown, const morda::Vec2f& pos, EMouseButton button, unsigned pointerId) override;

		bool OnMouseMove(const morda::Vec2f& pos, unsigned pointerId) override;
	};
	
public:
	Slider();
	
	Slider(const stob::Node& description);
	
public:
	ting::Signal1<float> factorChange;

	virtual ~Slider()noexcept{}
	
	float Factor()const noexcept{
		return this->curFactor;
	}
	
	void SetFactor(float newFactor);
	
private:
	void OnResize() override;
	
	morda::Vec2f ComputeMinDim()const noexcept override;
	
	void Render(const morda::Matr4f& matrix) const override;
};



}//~namespace
