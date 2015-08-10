/* The MIT License:

Copyright (c) 2011-2014 Ivan Gagis <igagis@gmail.com>

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
#	include <iostream>
#endif

#include <algorithm>

#include "Vector2.hpp"

#include "../config.hpp"

//Under Windows and MSVC compiler there are 'min' and 'max' macros defined for some reason, get rid of them.
#ifdef min
#	undef min
#endif
#ifdef max
#	undef max
#endif



namespace morda{



/**
 * @brief 2d axis-aligned rectangle class.
 */
template <class T> class Rectangle2{
public:
	typedef T T_Component;
	
	//TODO: doxygen
	Vector2<T> p; //Left-Bottom corner
	
	//TODO: doxygen
	Vector2<T> d; //dimensions
	
	//TODO: doxygen
	Rectangle2()noexcept{}
	
	//TODO: doxygen
	Rectangle2(T value)noexcept :
			p(value),
			d(value)
	{}
	
	//TODO: doxygen
	Rectangle2(T left, T bottom, T width, T height)noexcept :
			p(left, bottom),
			d(width, height)
	{}

	//TODO: doxygen
	Rectangle2(Vector2<T> leftBottom, Vector2<T> dimensions)noexcept :
			p(leftBottom),
			d(dimensions)
	{}
	
	template <class TT> explicit Rectangle2(const Rectangle2<TT>& r) :
			p(r.p),
			d(r.d)
	{}
	
	//TODO: doxygen
	Vector2<T> Center()const noexcept{
		return this->p + this->d / 2;
	}

	//TODO: doxygen
	void MoveCenterTo(const Vector2<T>& vec)noexcept{
		this->p = vec - this->d / 2;
	}

	//TODO: doxygen
	bool Overlaps(const Vector2<T>& vec)const noexcept{
		return
				vec.x < this->Right() &&
				vec.x >= this->Left() &&
				vec.y >= this->Bottom() &&
				vec.y < this->Top()
			;
	}
	
	//TODO: doxygen
	//Get intersection of two rectangles
	Rectangle2 Intersection(const Rectangle2& rect)const noexcept{
		return Rectangle2(*this).Intersect(rect);
	}
	
	//TODO: doxygen
	Rectangle2& Intersect(const Rectangle2& rect)noexcept{
		for(unsigned i = 0; i != 2; ++i){
			T end = std::min(this->p[i] + this->d[i], rect.p[i] + rect.d[i]);
			this->p[i] = std::max(this->p[i], rect.p[i]);
			if(end > this->p[i]){
				this->d[i] = end - this->p[i];
			}else{
				this->d[i] = 0;
			}
		}
		
		return *this;
	}
	
	//TODO: doxygen
	Vector2<T> Extent()const noexcept{
		return this->d / 2;
	}

	//TODO: doxygen
	Vector2<T> RightTop()const noexcept{
		return this->p + this->d;
	}

	//TODO: doxygen
	T& Left()noexcept{
		return this->p.x;
	}

	//TODO: doxygen
	const T& Left()const noexcept{
		return this->p.x;
	}

	//TODO: doxygen
	T Top()const noexcept{
		return this->p.y + this->d.y;
	}

	//TODO: doxygen
	T Right()const noexcept{
		return this->p.x + this->d.x;
	}

	//TODO: doxygen
	T& Bottom()noexcept{
		return this->p.y;
	}

	//TODO: doxygen
	const T& Bottom()const noexcept{
		return this->p.y;
	}
	
	//TODO: doxygen
	bool operator==(const Rectangle2& r)const noexcept{
		return this->p == r.p && this->d == r.d;
	}
	
	/**
	 * @brief Convert to Rectangle2 with different type of component.
	 * Convert this Rectangle2 to a Rectangle2 whose component type is different from T.
	 * Components are converted using constructor of target type passing the source
	 * component as argument of the target type constructor.
     * @return converted Vector2.
     */
	template <class TS> Rectangle2<TS> to()const noexcept{
		return Rectangle2<TS>(*this);
	}
	
#ifdef DEBUG  
	friend std::ostream& operator<<(std::ostream& s, const Rectangle2<T>& rect){
		s << "[" << rect.p << rect.d << "]";
		return s;
	}
#endif
};



//=====================
// Convenient typedefs
//=====================

typedef Rectangle2<float> Rect2f;

typedef Rectangle2<double> Rect2d;

typedef Rectangle2<int> Rect2i;

typedef Rectangle2<unsigned> Rect2u;

typedef Rectangle2<real> Rect2r;

}//~namespace
