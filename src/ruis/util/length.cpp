#include "length.hpp"

using namespace ruis;

real length::get(const context& ctx) const noexcept
{
	switch (this->value_units) {
		default:
			ASSERT(false)
			[[fallthrough]];
		case units::px:
			return this->value;
		case units::pp:
			return ctx.units.pp_to_px(this->value);
		case units::mm:
			return ctx.units.mm_to_px(this->value);
	}
}
