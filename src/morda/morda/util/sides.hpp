/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

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
