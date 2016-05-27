/* 
 * File:   Sides.hpp
 * Author: ivan
 *
 * Created on May 27, 2016, 5:02 PM
 */

#pragma once

#include <kolme/Rectangle.hpp>

namespace morda{

template <class T> class Sides : private kolme::Rectangle<T>{
	
public:
	Sides(){}
	
	Sides(T l, T t, T r, T b) :
			kolme::Rectangle<T>(l, b, r, t)
	{}
	
	T& left()noexcept{
		return this->p.x;
	}
	
	T& right()noexcept{
		return this->d.x;
	}
	
	T& bottom()noexcept{
		return this->p.y;
	}
	
	T& top()noexcept{
		return this->d.y;
	}
	
};

}
