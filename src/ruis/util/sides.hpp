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
 * @brief Template class holding some values associated with sides of a rectangle.
 * Values can be accessed by names of rectangle sides (left, top, right, bottom)
 * or by indices, in which case sides are:
 *   0 = left
 *   1 = top
 *   2 = right
 *   3 = bottom
 *
 * Commonly used for representing padding, margins, or other per-side dimensions.
 */
template <class component_type>
class sides : public std::array<component_type, 4>
{
public:
	/**
	 * @brief Default constructor.
	 * Does not initialize the side values.
	 */
	sides() = default;

	/**
	 * @brief Constructor that initializes all side values to the same value.
	 * @param v - value to assign to all four side values.
	 */
	sides(component_type v) :
		sides(v, v, v, v)
	{}

	/**
	 * @brief Constructor that initializes each side value individually.
	 * @param l - value for the left side value.
	 * @param t - value for the top side value.
	 * @param r - value for the right side value.
	 * @param b - value for the bottom side value.
	 */
	sides(
		component_type l, //
		component_type t,
		component_type r,
		component_type b
	) :
		std::array<component_type, 4>({
			{l, t, r, b}
    })
	{}

	/**
	 * @brief Get or set the left side value.
	 */
	component_type& left() noexcept
	{
		return this->operator[](0);
	}

	/**
	 * @brief Get the left side value.
	 */
	const component_type& left() const noexcept
	{
		return this->operator[](0);
	}

	/**
	 * @brief Get or set the top side value.
	 */
	component_type& top() noexcept
	{
		return this->operator[](1);
	}

	/**
	 * @brief Get the top side value.
	 */
	const component_type& top() const noexcept
	{
		return this->operator[](1);
	}

	/**
	 * @brief Get or set the right side value.
	 */
	component_type& right() noexcept
	{
		return this->operator[](2);
	}

	/**
	 * @brief Get the right side value.
	 */
	const component_type& right() const noexcept
	{
		return this->operator[](2);
	}

	/**
	 * @brief Get or set the bottom side value.
	 */
	component_type& bottom() noexcept
	{
		return this->operator[](3);
	}

	/**
	 * @brief Get the bottom side value.
	 */
	const component_type& bottom() const noexcept
	{
		return this->operator[](3);
	}

	/**
	 * @brief Sum of the left and right side values (horizontal extent).
	 * @return The combined x-dimension value.
	 */
	component_type dim_x() const noexcept
	{
		return this->left() + this->right();
	}

	/**
	 * @brief Sum of the top and bottom side values (vertical extent).
	 * @return The combined y-dimension value.
	 */
	component_type dim_y() const noexcept
	{
		return this->top() + this->bottom();
	}

	/**
	 * @brief Get the left-top corner.
	 * @return A 2D vector with x = left, y = top.
	 */
	r4::vector2<component_type> left_top() const noexcept
	{
		return {this->left(), this->top()};
	}

	/**
	 * @brief Get the right-top corner.
	 * @return A 2D vector with x = right, y = top.
	 */
	r4::vector2<component_type> right_top() const noexcept
	{
		return {this->right(), this->top()};
	}

	/**
	 * @brief Get the right-bottom corner.
	 * @return A 2D vector with x = right, y = bottom.
	 */
	r4::vector2<component_type> right_bottom() const noexcept
	{
		return {this->right(), this->bottom()};
	}

	/**
	 * @brief Get the left-bottom corner.
	 * @return A 2D vector with x = left, y = bottom.
	 */
	r4::vector2<component_type> left_bottom() const noexcept
	{
		return {this->left(), this->bottom()};
	}

	/**
	 * @brief The overall dimensions (width, height) implied by the sides.
	 * @return A 2D vector with x = dim_x(), y = dim_y().
	 */
	r4::vector2<component_type> dims() const noexcept
	{
		return {this->dim_x(), this->dim_y()};
	}

	/**
	 * @brief Check whether all sides are zero.
	 * @return true if every side value is zero, false otherwise.
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
	 * @brief Assign the same value to all sides.
	 * @param v - value to assign.
	 * @return Reference to this object.
	 */
	sides& operator=(component_type v) noexcept
	{
		for (auto& c : *this) {
			c = v;
		}
		return *this;
	}

	/**
	 * @brief Stream output operator.
	 * Prints the four side values in order: (left, top, right, bottom).
	 * @param stream - output stream.
	 * @param s - sides object to print.
	 * @return The output stream.
	 */
	friend std::ostream& operator<<(
		std::ostream& stream, //
		const sides<component_type>& s
	)
	{
		stream << "(" << s.left() << ", " << s.top() << ", " << s.right() << ", " << s.bottom() << ")";
		return stream;
	}

	/**
	 * @brief Operator less than.
	 * Implements lexicographic comparison for sides in the order:
	 * left, top, right, bottom.
	 * For use with std::map, std::set, etc.
	 * @param lhs - left hand side operand.
	 * @param rhs - right hand side operand.
	 * @return true if lhs < rhs, false otherwise.
	 */
	friend bool operator<(
		const sides& lhs, //
		const sides& rhs
	)
	{
		if (lhs.left() == rhs.left()) {
			if (lhs.top() == rhs.top()) {
				if (lhs.right() == rhs.right()) {
					return lhs.bottom() < rhs.bottom();
				}
				return lhs.right() < rhs.right();
			}
			return lhs.top() < rhs.top();
		}
		return lhs.left() < rhs.left();
	}
};

} // namespace ruis
