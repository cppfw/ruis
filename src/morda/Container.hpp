/* The MIT License:

Copyright (c) 2012 Ivan Gagis <igagis@gmail.com>

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

#include <vector>

#include "Widget.hpp"
#include "Layout.hpp"



namespace morda{



class Container : public Widget{
protected:
	typedef std::vector<ting::Ref<Widget> > T_ChildList;

private:
	T_ChildList children;

	ting::Ptr<Layout> layout;
	
	void ApplyDescription(const stob::Node& description);
	
protected:
	inline Container(){}
	
	inline Container(const stob::Node& description) :
			Widget(description)
	{
		this->ApplyDescription(description);
	}

public:
	~Container()throw(){}
	
	inline static ting::Ref<Container> New(const stob::Node& description){
		return ting::Ref<Container>(new Container(description));
	}
	
	inline static ting::Ref<Container> New(){
		return ting::Ref<Container>(new Container());
	}

	void SetLayout(ting::Ptr<Layout> layout){
		this->layout = layout;
	}
	
	//override
	void Render(const morda::Matr4f& matrix)const;

	//override
	bool OnMouseButtonDown(const morda::Vec2f& pos, EMouseButton button, unsigned pointerId);
	
	//override
	bool OnMouseButtonUp(const morda::Vec2f& pos, EMouseButton button, unsigned pointerId);
	
	//override
	bool OnMouseMove(const morda::Vec2f& pos, unsigned pointerId);
	
	//override
	void OnMouseOut();
	
	//override
	void OnResize();
	
	//override
	morda::Vec2f ComputeMinimalDimensions()const throw(){
		if(this->layout){
			return this->layout->ComputeMinimalDimensions(*this);
		}
		return this->Widget::ComputeMinimalDimensions();
	}

	void Add(const ting::Ref<Widget>& w);

	//return true if the widget was found in children and was removed
	bool Remove(const ting::Ref<Widget>& w);
	
protected:
//	inline const T_ChildList Children()const{
//		return this->children;
//	}
//
//	inline T_ChildList Children(){
//		return this->children;
//	}
};



}//~namespace
