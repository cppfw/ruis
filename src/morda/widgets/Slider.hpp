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

#include "Widget.hpp"
#include "Container.hpp"


namespace morda{


class Slider :
		public virtual morda::Widget,
		private morda::Container //users do not need to know that it is a container
{
	
	//no copying
	Slider(const Slider&);
	Slider& operator=(const Slider&);
	
	ting::Inited<float, 0> curFactor; //Current position from 0 to 1
	ting::Inited<float, 0> handleSizeFactor; //Current handle size factor from 0 to 1
	
	ting::Inited<bool, true> isVertical;
protected:
	Slider();
	
	Slider(const stob::Node& description);
	
public:
	static ting::Ref<Slider> New(const stob::Node& description){
		return ting::Ref<Slider>(new Slider(description));
	}
	
	static ting::Ref<Slider> New();

	virtual ~Slider()throw(){}
	
private:	
	void OnResize() OVERRIDE;
	
	morda::Vec2f ComputeMinDim()const throw() OVERRIDE;
	
	void Render(const morda::Matr4f& matrix) const OVERRIDE;
};



}//~namespace
