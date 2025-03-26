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

#include "length.hpp"

namespace ruis {

/**
 * @brief Layout dimension policy type.
 */
enum class dim {
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

/**
 * @brief Layout dimension request.
 * Can represent exact length value or one of [undefined, min, max, fill].
 */
class dimension
{
	dim type;
	length value;

public:
	/**
	 * @brief Construct a new dimension policy object.
	 * @param len - the exact length, undefined length value is equivalent to 'min'.
	 */
	constexpr dimension(length len) :
		type([&]() constexpr {
			if (len.is_undefined()) {
				return dim::min;
			} else {
				return dim::length;
			}
		}()),
		value(len)
	{}

	constexpr dimension(dim t = dim::undefined) :
		type(t)
	{}

	constexpr dim get_type() const noexcept
	{
		return this->type;
	}

	constexpr auto get_length() const noexcept
	{
		return this->value;
	}

	constexpr bool is_undefined() const noexcept
	{
		return this->get_type() == dim::undefined;
	}

	bool operator==(const length& d) const
	{
		switch (this->get_type()) {
			case dim::length:
				return this->value == d;
			case dim::min:
				return d.is_undefined();
			default:
				return false;
		}
	}

	bool operator!=(const length& d) const
	{
		return !this->operator==(d);
	}

	constexpr static dimension default_value()
	{
		return dim::min;
	}

	static dimension parse_value(const tml::forest& desc);
};

} // namespace ruis
