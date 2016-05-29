/* 
 * File:   Sides.hpp
 * Author: ivan
 *
 * Created on May 27, 2016, 5:02 PM
 */

#pragma once

#include <array>

namespace morda{

template <class T> class Sides : private std::array<T, 4>{
	
public:
	Sides(){}
	
	Sides(T l, T t, T r, T b) :
			std::array<T, 4>({{l, t, r, b}})
	{}
	
	T& left()noexcept{
		return this->operator[](0);
	}
	
	T& top()noexcept{
		return this->operator[](1);
	}
	
	T& right()noexcept{
		return this->operator[](2);
	}
	
	T& bottom()noexcept{
		return this->operator[](3);
	}
};

}
