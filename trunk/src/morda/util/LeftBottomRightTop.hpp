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

#include <stob/dom.hpp>

#include "Vector2.hpp"


namespace morda{



//TODO: doxygen
class LeftBottomRightTop{
public:
	Vec2f lb;
	Vec2f rt;
	
	LeftBottomRightTop(){}
	
	LeftBottomRightTop(float left, float bottom, float right, float top) :
			lb(left, bottom),
			rt(right, top)
	{}
	
	float& operator[](size_t i)noexcept{
		ASSERT(i < 4)
		return reinterpret_cast<float*>(this)[i];
	}
	
	const float& operator[](size_t i)const noexcept{
		ASSERT(i < 4)
		return reinterpret_cast<const float*>(this)[i];
	}
	
	/**
	 * @brief Parse the from STOB.
	 * Takes STOB node of the following form:
	 * @code
	 * padding{10 20 15 30}
	 * @endcode
	 * The order of values is as follows: left, bottom, right, top.
	 * If passed node has less than 4 child values then the rest values are
	 * filled with the last value encountered. For example
	 * @code
	 * padding{30 20}
	 * padding{5}
	 * @endcode
	 * is equivalent to
	 * @code
	 * padding{30 20 20 20}
	 * padding{5 5 5 5}
	 * @endcode
	 * Value of the root node does not matter, it is ignored.
     * @param node - STOB node to parse from.
     * @return Parsed LeftTopRightBottom object.
     */
	static LeftBottomRightTop FromSTOB(const stob::Node& node)noexcept;
	
	/**
	 * @brief Default values.
	 * Default values are all 0.
     * @return LeftTopRightBottom object initialized to default values.
     */
	static LeftBottomRightTop Default()noexcept{//TODO: remove that and add constructor with just one argument
		return LeftBottomRightTop(0, 0, 0, 0);
	}
};



}//~namespace
