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


#include "../Widget.hpp"
#include "../resources/ResFont.hpp"


namespace morda{



class Label : public Widget{
	std::string text;
	
	ting::Ref<morda::ResFont> font;
	
	tride::Vec2f pivot;
	
	void ApplyProperties(const stob::Node* properties);
	
protected:
	Label(ting::Ptr<stob::Node> properties) :
			Widget(properties)
	{	
		this->ApplyProperties(this->properties.operator->());
	}
	
	Label(const stob::Node* properties = 0) :
			Widget(properties)
	{
		this->ApplyProperties(properties);
	}
public:
	enum E_Gravity{
		CENTER,
		LEFT,
		TOP_LEFT,
		BOTTOM_LEFT,
		RIGHT,
		TOP_RIGHT,
		BOTTOM_RIGHT,
		TOP,
		BOTTOM
	};
	
private:
	E_Gravity gravity;
	
public:
	void SetGravity(E_Gravity gravity);
	
	~Label()throw(){}
	
	void SetText(const std::string& text);
	
	inline const std::string& GetText()const throw(){
		return this->text;
	}
	
	//override
	void Render(const tride::Matr4f& matrix)const;
		
	inline static ting::Ref<Label> New(ting::Ptr<stob::Node> properties){
		return ting::Ref<Label>(new Label(properties));
	}
	
	inline static ting::Ref<Label> New(const stob::Node* properties = 0){
		return ting::Ref<Label>(new Label(properties));
	}
};



}//~namespace
