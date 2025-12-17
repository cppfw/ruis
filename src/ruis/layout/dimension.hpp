/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include "../util/length.hpp"

namespace ruis::layout {

/**
 * @brief Layout dimension request.
 * Can represent exact length value or one of [undefined, min, max, fill].
 */
class dimension
{
public:
	/**
	 * @brief Layout dimension policy type.
	 */
	enum class type {
		/**
		 * @brief The dimension policy is undefined.
		 * Most of the layouts will default to 'min' dimension policy in this case.
		 */
		undefined,

		/**
		 * @brief Requests minimal dimensions of the widget.
		 * The widget will always be given minimal space it needs to properly draw.
		 */
		min,

		/**
		 * @brief Requests minimal or bigger dimensions of widget.
		 * The widget will be given at least minimal space it needs to properly draw.
		 * 'max' behaves the same way as 'min' during measure, but during layouting
		 * the widget will be given same size as parent.
		 */
		max,

		/**
		 * @brief Requests widget to be same size as its parent.
		 * Minimal size of the widget is assumed to be 0.
		 */
		fill,

		/**
		 * @brief Requests an exact specified length.
		 * The widget size will be set to the specified length.
		 */
		length
	};

private:
	type type_v;
	length value;

public:
	constexpr dimension(type t = type::undefined) :
		type_v(t)
	{}

	/**
	 * @brief Construct a new dimension policy object.
	 * @param len - the exact length, undefined length value is equivalent to 'min'.
	 */
	constexpr dimension(length len) :
		type_v([&]() constexpr {
			if (len.is_undefined()) {
				return type::min;
			} else {
				return type::length;
			}
		}()),
		value(len)
	{}

	constexpr type get_type() const noexcept
	{
		return this->type_v;
	}

	constexpr auto get_length() const noexcept
	{
		return this->value;
	}

	constexpr bool is_undefined() const noexcept
	{
		return this->get_type() == type::undefined;
	}

	bool operator==(const dimension& d) const
	{
		if (this->get_type() != d.get_type()) {
			return false;
		}

		switch (this->get_type()) {
			case type::min:
			case type::max:
			case type::fill:
			case type::undefined:
				return true;
			case type::length:
				return this->get_length() == d.get_length();
		}

		return false;
	}

	bool operator==(const length& d) const
	{
		switch (this->get_type()) {
			case type::length:
				return this->value == d;
			case type::min:
				return d.is_undefined();
			default:
				return false;
		}
	}

	bool operator!=(const length& d) const
	{
		return !this->operator==(d);
	}

	static dimension make_from(const tml::forest& desc);

	static const dimension min;
	static const dimension max;
	static const dimension fill;

	friend std::ostream& operator<<(std::ostream& o, const dimension& d);
};

std::ostream& operator<<(std::ostream& o, const dimension& d);

} // namespace ruis::layout

namespace ruis {
using dim = layout::dimension;
} // namespace ruis
