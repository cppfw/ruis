/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include <r4/vector.hpp>

namespace ruis {

/**
 * @brief Template class holding some values associated with corners of a rectangle.
 * Values can be accessed by names of rectangle corners (left_top, right_top, right_bottom, left_bottom)
 * or by indices, in which case corners are:
 *   0 = left_top
 *   1 = right_top
 *   2 = right_bottom
 *   3 = left_bottom
 */
template <class component_type>
class corners : public std::array<component_type, 4>
{
public:
	corners() = default;

	corners(component_type b) :
		corners(b, b, b, b)
	{}

	corners(
		component_type left_top, //
		component_type right_top,
		component_type right_bottom,
		component_type left_bottom
	) :
		std::array<component_type, 4>({
			{left_top, right_top, right_bottom, left_bottom}
		})
	{}

	component_type& left_top() noexcept
	{
		return this->operator[](0);
	}

	const component_type& left_top() const noexcept
	{
		return this->operator[](0);
	}

	component_type& right_top() noexcept
	{
		return this->operator[](1);
	}

	const component_type& right_top() const noexcept
	{
		return this->operator[](1);
	}

	component_type& right_bottom() noexcept
	{
		return this->operator[](2);
	}

	const component_type& right_bottom() const noexcept
	{
		return this->operator[](2);
	}

	component_type& left_bottom() noexcept
	{
		return this->operator[](3);
	}

	const component_type& left_bottom() const noexcept
	{
		return this->operator[](3);
	}

	component_type top() const noexcept
	{
		return this->left_top() + this->right_top();
	}

	component_type bottom() const noexcept
	{
		return this->left_bottom() + this->right_bottom();
	}

	component_type left() const noexcept
	{
		return this->left_top() + this->left_bottom();
	}

	component_type right() const noexcept
	{
		return this->right_top() + this->right_bottom();
	}

	bool is_zero() const noexcept
	{
		for (auto& e : *this) {
			if (e != 0) {
				return false;
			}
		}
		return true;
	}

	corners& operator=(component_type e) noexcept
	{
		for (auto& v : *this) {
			v = e;
		}
		return *this;
	}

	friend std::ostream& operator<<(
		std::ostream& stream, //
		const corners<component_type>& c
	)
	{
		stream << "("
			<< c.left_top() << ", "
			<< c.right_top() << ", "
			<< c.right_bottom() << ", "
			<< c.left_bottom() << ")";
		return stream;
	}

	/**
	 * @brief Operator less than.
	 * Implements operation < for corners.
	 * For use with std::map, etc.
	 * @param lhs - left hand side operand.
	 * @param rhs - right hand side operand.
	 * @return true if lhs < rhs.
	 * @return false otherwise.
	 */
	friend bool operator<(
		const corners& lhs, //
		const corners& rhs
	)
	{
		if (lhs.left_top() == rhs.left_top()) {
			if (lhs.right_top() == rhs.right_top()) {
				if (lhs.right_bottom() == rhs.right_bottom()) {
					return lhs.left_bottom() < rhs.left_bottom();
				}
				return lhs.right_bottom() < rhs.right_bottom();
			}
			return lhs.right_top() < rhs.right_top();
		}
		return lhs.left_top() < rhs.left_top();
	}
};

} // namespace ruis
