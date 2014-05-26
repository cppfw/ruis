/* The MIT License:

Copyright (c) 2012-2014 Ivan Gagis <igagis@gmail.com>

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


#include "LeftBottomRightTop.hpp"



namespace morda{



class Padded{
	LeftBottomRightTop padding;
protected:
	inline Padded() :
			padding(LeftBottomRightTop::Default())
	{}
	
	inline Padded(LeftBottomRightTop padding) :
			padding(padding)
	{}
	
	inline Padded(const stob::Node& description){
		const stob::Node* n = description.Child("padding").node();
		if(!n){
			this->padding = LeftBottomRightTop::Default();
			return;
		}
		this->padding = LeftBottomRightTop::FromSTOB(*n);
	}
	
public:
	inline void SetPadding(LeftBottomRightTop padding)throw(){
		this->padding = padding;
	}
	
	inline const LeftBottomRightTop& Padding()const throw(){
		return this->padding;
	}
};



}//~namespace
