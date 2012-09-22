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
	
	void ApplyDescription(const stob::Node& description);
protected:
	inline Button(const stob::Node& description, bool doNotCopyProp) :
			AbstractButton(description, doNotCopyProp),
			label(morda::Label::New(description, true))
	{
		this->ApplyDescription(description);
	}
	
	inline Button(){}
public:
	
	~Button()throw(){}
	
	//override
	void Render(const morda::Matr4f& matrix)const;
	
	//override
	morda::Vec2f ComputeMinDim()const throw();
	
	//override
	void OnResize();
	
	void SetText(const std::string& text);
	
	inline static ting::Ref<Button> New(const stob::Node& description, bool doNotCopyProp){
		return ting::Ref<Button>(new Button(description, doNotCopyProp));
	}
	
	inline static ting::Ref<Button> New(){
		return ting::Ref<Button>(new Button());
	}
};



}//~namespace
