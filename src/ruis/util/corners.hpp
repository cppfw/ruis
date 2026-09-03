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

namespace ruis {

/**
 * @brief Template class holding some values associated with corners of a rectangle.
 * Values can be accessed by names of rectangle corners (left_top, right_top, right_bottom, left_bottom)
 * or by indices, in which case corners are:
 *   0 = left_top
 *   1 = right_top
 *   2 = right_bottom
 *   3 = left_bottom
 *
 * The corner ordering follows a clockwise traversal starting from the top-left corner.
 */
template <class component_type>
class corners : public std::array<component_type, 4>
{
public:
	/**
	 * @brief Default constructor.
	 * The corner values are uninitialized.
	 */
	corners() = default;

	/**
	 * @brief Constructor that initializes all corners to the same value.
	 * @param v - value to assign to all four corners.
	 */
	corners(component_type v) :
		corners(v, v, v, v)
	{}

	/**
	 * @brief Constructor that initializes each corner individually.
	 * @param left_top - value for the left-top corner.
	 * @param right_top - value for the right-top corner.
	 * @param right_bottom - value for the right-bottom corner.
	 * @param left_bottom - value for the left-bottom corner.
	 */
	corners(
		component_type left_top, //
		component_type right_top,
		component_type right_bottom,
		component_type left_bottom
	) :
		std::array<component_type, 4>({
			{//
			 left_top, //
			 right_top, right_bottom,
			 left_bottom
			}
    })
	{}

	/**
	 * @brief Get or set the left-top corner value.
	 */
	component_type& left_top() noexcept
	{
		return this->operator[](0);
	}

	/**
	 * @brief Get the left-top corner value.
	 */
	const component_type& left_top() const noexcept
	{
		return this->operator[](0);
	}

	/**
	 * @brief Get or set the right-top corner value.
	 */
	component_type& right_top() noexcept
	{
		return this->operator[](1);
	}

	/**
	 * @brief Get the right-top corner value.
	 */
	const component_type& right_top() const noexcept
	{
		return this->operator[](1);
	}

	/**
	 * @brief Get or set the right-bottom corner value.
	 */
	component_type& right_bottom() noexcept
	{
		return this->operator[](2);
	}

	/**
	 * @brief Get the right-bottom corner value.
	 */
	const component_type& right_bottom() const noexcept
	{
		return this->operator[](2);
	}

	/**
	 * @brief Get or set the left-bottom corner value.
	 */
	component_type& left_bottom() noexcept
	{
		return this->operator[](3);
	}

	/**
	 * @brief Get the left-bottom corner value.
	 */
	const component_type& left_bottom() const noexcept
	{
		return this->operator[](3);
	}

	/**
	 * @brief Check whether all corner values are zero.
	 * @return true if every corner value is zero, false otherwise.
	 */
	bool is_zero() const noexcept
	{
		for (auto& e : *this) {
			if (e != 0) {
				return false;
			}
		}
		return true;
	}

	/**
	 * @brief Assign the same value to all corners.
	 * @param v - value to assign.
	 * @return Reference to this object.
	 */
	corners& operator=(component_type v) noexcept
	{
		for (auto& c : *this) {
			c = v;
		}
		return *this;
	}

	/**
	 * @brief Stream output operator.
	 * Prints the four corner values in order: (left_top, right_top, right_bottom, left_bottom).
	 * @param s - output stream.
	 * @param c - corners object to print.
	 * @return The output stream.
	 */
	friend std::ostream& operator<<(
		std::ostream& s, //
		const corners<component_type>& c
	)
	{
		s //
			<< "(" //
			<< c.left_top() << ", " //
			<< c.right_top() << ", " //
			<< c.right_bottom() << ", " //
			<< c.left_bottom() //
			<< ")";
		return s;
	}

	/**
	 * @brief Operator less than.
	 * Implements lexicographic comparison for corners in the order:
	 * left_top, right_top, right_bottom, left_bottom.
	 * For use with std::map, std::set, etc.
	 * @param lhs - left hand side operand.
	 * @param rhs - right hand side operand.
	 * @return true if lhs < rhs, false otherwise.
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
