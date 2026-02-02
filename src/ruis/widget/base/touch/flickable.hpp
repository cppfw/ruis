#pragma once

#include "../../widget.hpp"

namespace ruis::touch {

class flickable : virtual public ruis::widget
{
	constexpr static auto scroll_threshold_pp = 5;

	// TODO: convert from scroll_threshold_pp in on_reload()?
	// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, "constant value")
	real scroll_threshold_px = 10;

	enum class state {
		idle,
		within_scroll_threshold,
		not_scrolling,
		scrolling
	} cur_state = state::idle;

	vec2 prev_touch_point;

	// Scroll area only works in single touch mode.
	// This variable holds the pointer ID of the current touch.
	unsigned cur_pointer_id = std::numeric_limits<unsigned>::max();

public:
	bool on_mouse_button(const mouse_button_event& event) override;
	bool on_mouse_move(const mouse_move_event& event) override;

protected:
    // TODO: doxygen
	virtual bool flickable_on_mouse_button(const mouse_button_event& event) = 0;

	virtual bool flickable_on_mouse_move(const mouse_move_event& event) = 0;

	virtual void flickable_scroll_by(const vec2& delta) = 0;
};

} // namespace ruis::touch
