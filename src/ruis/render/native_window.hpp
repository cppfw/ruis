#pragma once

#include <list>

#include "../util/mouse_cursor.hpp"

namespace ruis::render {
class context;

// TODO: doxygen
class native_window
{
	friend class ruis::render::context;

	bool is_fullscreen_v = false;

	virtual void bind_rendering_context() {}

	virtual void set_fullscreen_internal(bool enable) {}

	std::list<mouse_cursor> cursor_stack = {mouse_cursor::arrow};

	virtual void set_mouse_cursor(ruis::mouse_cursor c) {}

public:
	virtual ~native_window() = default;

	// TODO: make private somehow?
	virtual void swap_frame_buffers() {}

	void set_fullscreen(bool enable);

	bool is_fullscreen() const noexcept
	{
		return this->is_fullscreen_v;
	}

	// ================
	// = mouse cursor =

	using cursor_id = decltype(cursor_stack)::iterator;

	cursor_id push_mouse_cursor(mouse_cursor cursor);
	void pop_mouse_cursor(cursor_id i);

	virtual void set_mouse_cursor_visible(bool visible) {}
};
} // namespace ruis::render
