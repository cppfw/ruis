/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include <tml/tree.hpp>

#include "../config.hpp"
#include "../util/length.hpp"
#include "../util/units.hpp"
#include "../util/align.hpp"

namespace ruis {

/**
 * @brief Layout parameters.
 */
struct layout_parameters {
	class dimension
	{
	public:
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
		/**
		 * @brief Construct a new dimension policy object.
		 * @param dim - the exact length, undefined length value is equivalent to 'min'.
		 */
		dimension(length dim) :
			type_v([&]() {
				if (dim.is_undefined()) {
					return type::min;
				} else {
					return type::length;
				}
			}()),
			value(dim)
		{}

		dimension(type t = type::undefined) :
			type_v(t)
		{}

		type get_type() const noexcept
		{
			return this->type_v;
		}

		auto get_length() const noexcept
		{
			ASSERT(this->get_type() == type::length)
			return this->value;
		}

		bool is_undefined() const noexcept
		{
			return this->type_v == type::undefined;
		}

		bool operator==(const length& d) const
		{
			if (this->type_v == type::length) {
				return this->value == d;
			} else if (this->type_v == type::min) {
				return d.is_undefined();
			}
			return false;
		}

		bool operator!=(const length& d) const
		{
			return !this->operator==(d);
		}
	};

	const static dimension min;
	const static dimension max;
	const static dimension fill;

	/**
	 * @brief desired dimensions.
	 * Components should hold non-negative value in pixels or [min, max, fill].
	 */
	r4::vector2<dimension> dims;

	/**
	 * @brief Weight of the widget.
	 * Weight defines how much space widget occupies in addition to its minimal or explicitly set size.
	 * Default weight is 0, which means that the widget will not occupy extra space.
	 * Value less than 0 is invalid, default weight will be assumed.
	 */
	real weight = -1;

	/**
	 * @brief Alignment of the widget within its parent.
	 * Horizontal and vertical alignment.
	 */
	r4::vector2<ruis::align> align = {ruis::align::undefined, ruis::align::undefined};

	static layout_parameters make(const tml::forest& desc, const ruis::units& units);
};

using lp = layout_parameters;

}; // namespace ruis
