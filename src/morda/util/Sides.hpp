/* 
 * File:   Sides.hpp
 * Author: ivan
 *
 * Created on May 27, 2016, 5:02 PM
 */

#pragma once

#include <array>
#include <ostream>

namespace morda{

template <class T> class Sides : public std::array<T, 4>{
	
public:
	Sides(){}
	
	Sides(T l, T t, T r, T b) :
			std::array<T, 4>({{l, t, r, b}})
	{}
	
	T& left()noexcept{
		return this->operator[](0);
	}
	
	const T& left()const noexcept{
		return this->operator[](0);
	}
	
	T& top()noexcept{
		return this->operator[](1);
	}
	
	const T& top()const noexcept{
		return this->operator[](1);
	}
	
	T& right()noexcept{
		return this->operator[](2);
	}
	
	const T& right()const noexcept{
		return this->operator[](2);
	}
	
	T& bottom()noexcept{
		return this->operator[](3);
	}
	
	const T& bottom()const noexcept{
		return this->operator[](3);
	}
	
	friend std::ostream& operator<<(std::ostream& s, const Sides<T>& sides){
		s << "(" << sides.left() << ", " << sides.top() << ", " << sides.right() << ", " << sides.bottom() << ")";
		return s;
	}
};

}
