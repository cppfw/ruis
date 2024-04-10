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

#include <variant>

#include <tml/tree.hpp>

#include "../config.hpp"
#include "../util/units.hpp"

namespace ruis {

/**
 * @brief Layout parameters.
 */
struct lp {
	/**
	 * @brief Alignment parameter values.
	 */
	enum class align {
		/**
		 * @brief Undefined alignment.
		 * In case alignemnt is undefined the default center alignment is assumed.
		 */
		undefined,

		/**
		 * @brief Left or top alignment.
		 */
		front,

		/**
		 * @brief Center lignment.
		 */
		center,

		/**
		 * @brief Right or bottom alignment.
		 */
		back
	};

	class dimension_policy
	{
	public:
		enum class special {
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
			fill
		};

	private:
		struct undefined {
			bool operator==(const undefined&) const noexcept
			{
				return true;
			}
		};

		// TODO: use length
		std::variant<undefined, special, real> value;

	public:
		dimension_policy() :
			value(undefined())
		{}

		/**
		 * @brief Construct a new dimension policy object.
		 * @param number - number in pixels, negative number is equivalent to 'min' special value.
		 */
		dimension_policy(real number) :
			value([&]() -> decltype(value) {
				if (number < 0) {
					return special::min;
				} else {
					return number;
				}
			}())
		{}

		dimension_policy(special special_value) :
			value(special_value)
		{}

		bool is_min() const noexcept
		{
			return std::holds_alternative<special>(this->value) && *std::get_if<special>(&this->value) == special::min;
		}

		bool is_max() const noexcept
		{
			return std::holds_alternative<special>(this->value) && *std::get_if<special>(&this->value) == special::max;
		}

		bool is_fill() const noexcept
		{
			return std::holds_alternative<special>(this->value) && *std::get_if<special>(&this->value) == special::fill;
		}

		bool is_number() const noexcept
		{
			return std::holds_alternative<real>(this->value);
		}

		real get_number() const
		{
			ASSERT(this->is_number())
			return std::get<real>(this->value);
		}

		bool is_undefined() const noexcept
		{
			return std::holds_alternative<undefined>(this->value);
		}

		bool operator==(const dimension_policy& p) const
		{
			return this->value == p.value;
		}

		bool operator!=(const dimension_policy& p) const
		{
			return !this->operator==(p);
		}

		bool operator==(real number) const
		{
			if (this->is_number()) {
				return this->get_number() == number;
			} else if (this->is_min()) {
				return number < 0;
			}
			return false;
		}

		bool operator!=(real number) const
		{
			return !this->operator==(number);
		}
	};

	const static dimension_policy min;
	const static dimension_policy max;
	const static dimension_policy fill;

	/**
	 * @brief desired dimensions.
	 * Components should hold non-negative value in pixels or [min, max, fill].
	 */
	r4::vector2<dimension_policy> dims;

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
	r4::vector2<align> align = {align::undefined, align::undefined};

	static lp make(const tml::forest& desc, const ruis::units& units);
};

}; // namespace ruis
