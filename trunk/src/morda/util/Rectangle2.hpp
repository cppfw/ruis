/* The MIT License:

Copyright (c) 2011-2012 Ivan Gagis <igagis@gmail.com>

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

// Homepage: http://morda.googlecode.com

/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#ifdef DEBUG
#include <iostream>
#endif

#include "Vector3.hpp"



namespace morda{



/**
 * @brief 2d axis-aligned rectangle class.
 */
template <class T> class Rectangle2{
public:
	Vector2<T> p; //Left-Bottom corner
	Vector2<T> d; //dimensions
	
	inline Rectangle2()throw(){}
	
	inline Rectangle2(T left, T bottom, T width, T height)throw() :
			p(left, bottom),
			d(width, height)
	{}

	inline Rectangle2(Vector2<T> leftBottom, Vector2<T> dimensions)throw() :
			p(leftBottom),
			d(dimensions)
	{}
	
	inline Vector2<T> Center()const throw(){
		return this->p + this->d / 2;
	}

	inline void MoveCenterTo(const Vector2<T>& vec)throw(){
		this->p = vec - this->d / 2;
	}

	bool Overlaps(const Vector2<T>& vec)const throw(){
		return
				vec.x < this->Right() &&
				vec.x >= this->Left() &&
				vec.y >= this->Bottom() &&
				vec.y < this->Top()
			;
	}
	
	inline Vector2<T> Extent()const throw(){
		return this->d / 2;
	}

	inline Vector2<T> RightTop()const throw(){
		return this->p + this->d;
	}

	inline T& Left()throw(){
		return this->p.x;
	}

	inline const T& Left()const throw(){
		return this->p.x;
	}

	inline T Top()const throw(){
		return this->p.y + this->d.y;
	}

	inline T Right()const throw(){
		return this->p.x + this->d.x;
	}

	inline T& Bottom()throw(){
		return this->p.y;
	}

	inline const T& Bottom()const throw(){
		return this->p.y;
	}
	
	inline bool operator==(const Rectangle2& r)const throw(){
		return this->p == r.p && this->d == r.d;
	}
	
#ifdef DEBUG  
	friend std::ostream& operator<<(std::ostream& s, const Rectangle2<T>& rect){
		s << "[" << rect.p << rect.d << "]";
		return s;
	}
#endif
};



//
//
// Some convenient typedefs
//
//

typedef Rectangle2<float> Rect2f;
typedef Rectangle2<double> Rect2d;
typedef Rectangle2<int> Rect2i;

}//~namespace
