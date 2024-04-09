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

public:
	length() :
		value(-1),
		value_units(units::px)
	{}

	length(real value) :
		value(value),
		value_units(units::px)
	{}

	length(real value, units u) :
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

	real get(const context& ctx) const noexcept;

	static length make_px(real value)
	{
		return length(value, units::px);
	}

	static length make_pp(real value)
	{
		return length(value, units::pp);
	}

	static length make_mm(real value)
	{
		return length(value, units::mm);
	}
};

} // namespace ruis
