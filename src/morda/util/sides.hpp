#pragma once

#include <array>
#include <ostream>

namespace morda{

/**
 * @brief Template class holding some values associated with sides of a rectangle.
 * Values can be accessed by names of rectangle sides (left, top, right, bottom)
 * or by indices, in which case sides are: 0 = left, 1 = top, 2 = right, 3 = bottom.
 */
template <class T> class sides : public std::array<T, 4>{

public:
	sides(){}

	sides(T b) :
			sides(b, b, b, b)
	{}

	sides(T l, T t, T r, T b) :
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

	friend std::ostream& operator<<(std::ostream& stream, const sides<T>& s){
		stream << "(" << s.left() << ", " << s.top() << ", " << s.right() << ", " << s.bottom() << ")";
		return stream;
	}
};

}
