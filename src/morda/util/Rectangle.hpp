/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <iostream>

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
template <class T> class Rectangle{
public:
	typedef T T_Component;
	
	//TODO: doxygen
	Vector2<T> p; //Left-Bottom corner
	
	//TODO: doxygen
	Vector2<T> d; //dimensions
	
	//TODO: doxygen
	Rectangle()noexcept{}
	
	//TODO: doxygen
	Rectangle(T value)noexcept :
			p(value),
			d(value)
	{}
	
	//TODO: doxygen
	Rectangle(T left, T bottom, T width, T height)noexcept :
			p(left, bottom),
			d(width, height)
	{}

	//TODO: doxygen
	Rectangle(Vector2<T> leftBottom, Vector2<T> dimensions)noexcept :
			p(leftBottom),
			d(dimensions)
	{}
	
	template <class TT> explicit Rectangle(const Rectangle<TT>& r) :
			p(r.p),
			d(r.d)
	{}
	
	//TODO: doxygen
	Vector2<T> center()const noexcept{
		return this->p + this->d / 2;
	}

	//TODO: doxygen
	void moveCenterTo(const Vector2<T>& vec)noexcept{
		this->p = vec - this->d / 2;
	}

	//TODO: doxygen
	bool overlaps(const Vector2<T>& vec)const noexcept{
		return
				vec.x < this->right() &&
				vec.x >= this->left() &&
				vec.y >= this->bottom() &&
				vec.y < this->top()
			;
	}
	
	//TODO: doxygen
	//Get intersection of two rectangles
	Rectangle intersection(const Rectangle& rect)const noexcept{
		return Rectangle(*this).intersect(rect);
	}
	
	//TODO: doxygen
	Rectangle& intersect(const Rectangle& rect)noexcept{
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
	Vector2<T> extent()const noexcept{
		return this->d / 2;
	}

	//TODO: doxygen
	Vector2<T> rightTop()const noexcept{
		return this->p + this->d;
	}

	//TODO: doxygen
	T& left()noexcept{
		return this->p.x;
	}

	//TODO: doxygen
	const T& left()const noexcept{
		return this->p.x;
	}

	//TODO: doxygen
	T top()const noexcept{
		return this->p.y + this->d.y;
	}

	//TODO: doxygen
	T right()const noexcept{
		return this->p.x + this->d.x;
	}

	//TODO: doxygen
	T& bottom()noexcept{
		return this->p.y;
	}

	//TODO: doxygen
	const T& bottom()const noexcept{
		return this->p.y;
	}
	
	//TODO: doxygen
	bool operator==(const Rectangle& r)const noexcept{
		return this->p == r.p && this->d == r.d;
	}
	
	/**
	 * @brief Convert to Rectangle2 with different type of component.
	 * Convert this Rectangle2 to a Rectangle2 whose component type is different from T.
	 * Components are converted using constructor of target type passing the source
	 * component as argument of the target type constructor.
     * @return converted Vector2.
     */
	template <class TS> Rectangle<TS> to()const noexcept{
		return Rectangle<TS>(*this);
	}
	
	friend std::ostream& operator<<(std::ostream& s, const Rectangle<T>& rect){
		s << "[" << rect.p << rect.d << "]";
		return s;
	}
};



//=====================
// Convenient typedefs
//=====================

typedef Rectangle<float> Rectf;

typedef Rectangle<double> Rectd;

typedef Rectangle<int> Recti;

typedef Rectangle<unsigned> Rectu;

typedef Rectangle<real> Rectr;

}//~namespace
