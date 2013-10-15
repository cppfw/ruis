/* The MIT License:

Copyright (c) 2012-2013 Ivan Gagis

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

#include "Button.hpp"
#include "Label.hpp"


namespace morda{



class TextButton : public AbstractButton, public Label{
	void ApplyDescription(const stob::Node& description);
protected:
	inline TextButton(const stob::Node& description, bool doNotCopyProp) :
			Widget(description, doNotCopyProp),
			AbstractButton(description, doNotCopyProp),
			Label(description, doNotCopyProp)
	{
		this->ApplyDescription(description);
	}
	
	inline TextButton(){}
public:
	
	~TextButton()throw(){}
	
	//override
	void Render(const morda::Matr4f& matrix)const;
	
	//override
	void OnResize();
	
	inline static ting::Ref<TextButton> New(const stob::Node& description, bool doNotCopyProp){
		return ting::Ref<TextButton>(new TextButton(description, doNotCopyProp));
	}
	
	inline static ting::Ref<TextButton> New(){
		return ting::Ref<TextButton>(new TextButton());
	}
};



}//~namespace
