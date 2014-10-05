/* The MIT License:

Copyright (c) 2014 Ivan Gagis

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

#include "Widget.hpp"


namespace morda{


class LinearWidget : public virtual Widget{
	LinearWidget(const LinearWidget&);
	LinearWidget& operator=(const LinearWidget&);
	
	bool isVertical;
	
	bool isReverse;

protected:
	LinearWidget(const stob::Node* desc = nullptr);

	
	//TODO: doxygen
	unsigned GetLongIndex()const NOEXCEPT{
		return this->isVertical ? 1 : 0;
	}

	//TODO: doxygen
	unsigned GetTransIndex()const NOEXCEPT{
		return this->isVertical ? 0 : 1;
	}

public:	
	void SetVertical(bool isVertical)NOEXCEPT{
		this->isVertical = isVertical;
	}
	
	bool IsVertical()const NOEXCEPT{
		return this->isVertical;
	}
	
	void SetReverse(bool reverse)NOEXCEPT{
		this->isReverse = reverse;
	}
	
	bool IsReverse()const NOEXCEPT{
		return this->isReverse;
	}
private:

};

}
