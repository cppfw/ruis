#pragma once

#include "../context.hpp"

namespace ruis {

class length
{
	real value;

public:
	enum class units {
		px,
		pp,
		mm
	};

private:
	units value_units;

	real get_internal(const context& ctx) const noexcept;

public:
	constexpr length() :
		value(-1),
		value_units(units::px)
	{}

	constexpr length(real value) :
		value(value),
		value_units(units::px)
	{}

	constexpr length(real value, units u) :
		value(value),
		value_units(u)
	{}

	bool is_valid() const noexcept
	{
		return this->value >= real(0);
	}

	real get(const utki::shared_ref<const context>& ctx) const noexcept
	{
		return this->get(ctx.get());
	}

	real get(const context& ctx) const noexcept
	{
		ASSERT(this->is_valid())
		return this->get_internal(ctx);
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
};

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
