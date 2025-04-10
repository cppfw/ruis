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

#include <tml/tree.hpp>
#include <utki/shared_ref.hpp>

#include "units.hpp"

namespace ruis {

class context;

class length
{
	real value = -1;

public:
	enum class units {
		px,
		pp,
		mm
	};

private:
	units value_units = units::px;

	real get_internal(const ruis::units& units) const noexcept;

public:
	constexpr length() = default;

	constexpr length(real value) :
		value(value)
	{}

	constexpr length(real value, units u) :
		value(value),
		value_units(u)
	{}

	constexpr bool is_undefined() const noexcept
	{
		return this->value < real(0);
	}

	real get(const context& ctx) const noexcept;

	real get(const ruis::units& units) const noexcept
	{
		ASSERT(!this->is_undefined())
		return this->get_internal(units);
	}

	constexpr bool operator==(const length& l) const noexcept
	{
		return this->value == l.value && this->value_units == l.value_units;
	}

	constexpr static length make_px(real value)
	{
		return {value, units::px};
	}

	constexpr static length make_pp(real value)
	{
		return {value, units::pp};
	}

	constexpr static length make_mm(real value)
	{
		return {value, units::mm};
	}

	static length make_from(const tml::forest& desc);

	friend std::ostream& operator<<(std::ostream& o, const length& l);
};

std::ostream& operator<<(std::ostream& o, const length& l);

using vec2_length = r4::vector2<length>;

namespace length_literals {

inline constexpr length operator""_px(unsigned long long value)
{
	return length::make_px(real(value));
}

inline constexpr length operator""_px(long double value)
{
	return length::make_px(real(value));
}

inline constexpr length operator""_pp(unsigned long long value)
{
	return length::make_pp(real(value));
}

inline constexpr length operator""_pp(long double value)
{
	return length::make_pp(real(value));
}

inline constexpr length operator""_mm(unsigned long long value)
{
	return length::make_mm(real(value));
}

inline constexpr length operator""_mm(long double value)
{
	return length::make_mm(real(value));
}

} // namespace length_literals

} // namespace ruis
