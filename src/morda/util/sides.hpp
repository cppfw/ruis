/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

namespace morda {

/**
 * @brief Template class holding some values associated with sides of a rectangle.
 * Values can be accessed by names of rectangle sides (left, top, right, bottom)
 * or by indices, in which case sides are: 0 = left, 1 = top, 2 = right, 3 = bottom.
 */
template <class component_type>
class sides : public std::array<component_type, 4>
{
public:
	sides() = default;

	sides(component_type b) :
		sides(b, b, b, b)
	{}

	sides(component_type l, component_type t, component_type r, component_type b) :
		std::array<component_type, 4>({
			{l, t, r, b}
    })
	{}

	component_type& left() noexcept
	{
		return this->operator[](0);
	}

	const component_type& left() const noexcept
	{
		return this->operator[](0);
	}

	component_type& top() noexcept
	{
		return this->operator[](1);
	}

	const component_type& top() const noexcept
	{
		return this->operator[](1);
	}

	component_type& right() noexcept
	{
		return this->operator[](2);
	}

	const component_type& right() const noexcept
	{
		return this->operator[](2);
	}

	component_type& bottom() noexcept
	{
		return this->operator[](3);
	}

	const component_type& bottom() const noexcept
	{
		return this->operator[](3);
	}

	friend std::ostream& operator<<(std::ostream& stream, const sides<component_type>& s)
	{
		stream << "(" << s.left() << ", " << s.top() << ", " << s.right() << ", " << s.bottom() << ")";
		return stream;
	}
};

} // namespace morda
