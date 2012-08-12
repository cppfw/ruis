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

#include <string>

#include "AbstractButton.hpp"
#include "Label.hpp"


namespace morda{



class Button : public AbstractButton{
	ting::Ref<morda::Label> label;
	
	void ApplyProperties(const stob::Node* properties);
protected:
	inline Button(ting::Ptr<stob::Node> properties) :
			AbstractButton(properties),
			label(morda::Label::New(this->properties.operator->()))
	{
		this->ApplyProperties(this->properties.operator->());
	}
	
	inline Button(const stob::Node* properties = 0) :
			AbstractButton(properties),
			label(morda::Label::New(properties))
	{
		this->ApplyProperties(properties);
	}
public:
	
	~Button()throw(){}
	
	//override
	void Render(const tride::Matr4f& matrix)const;
	
	void SetText(const std::string& text);
	
	inline static ting::Ref<Button> New(ting::Ptr<stob::Node> properties){
		return ting::Ref<Button>(new Button(properties));
	}
	
	inline static ting::Ref<Button> New(const stob::Node* properties = 0){
		return ting::Ref<Button>(new Button(properties));
	}
};



}//~namespace
