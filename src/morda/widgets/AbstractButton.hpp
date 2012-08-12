/* The MIT License:

Copyright (c) 2012 Ivan Gagis

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

#include <ting/Signal.hpp>

#include "../Widget.hpp"



namespace morda{



class AbstractButton : public Widget{
	ting::Inited<bool, false> isPressed;
	
	void ApplyProperties(const stob::Node* properties);
protected:
	inline AbstractButton(ting::Ptr<stob::Node> properties) :
			Widget(properties)
	{
		this->ApplyProperties(this->properties.operator->());
	}
			
	inline AbstractButton(const stob::Node* properties = 0) :
			Widget(properties)
	{
		this->ApplyProperties(properties);
	}
public:
	
	~AbstractButton()throw(){}
	
	ting::Signal0 pressed;
	
	//override
	void Render(const tride::Matr4f& matrix)const;
	
	//override
	bool OnMouseButtonDown(const tride::Vec2f& pos, EMouseButton button, unsigned pointerId);
	
	//override
	bool OnMouseButtonUp(const tride::Vec2f& pos, EMouseButton button, unsigned pointerId);
	
	//override
	void OnMouseOut();
	
	inline bool IsPressed()const throw(){
		return this->isPressed;
	}
	
	inline static ting::Ref<AbstractButton> New(ting::Ptr<stob::Node> properties){
		return ting::Ref<AbstractButton>(new AbstractButton(properties));
	}
	
	inline static ting::Ref<AbstractButton> New(stob::Node* properties = 0){
		return ting::Ref<AbstractButton>(new AbstractButton(properties));
	}
};



}//~namespace
