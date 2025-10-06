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

#include <functional>
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
	native_window() = default;

	native_window(const native_window&) = delete;
	native_window& operator=(const native_window&) = delete;

	native_window(native_window&&) = delete;
	native_window& operator=(native_window&&) = delete;

	virtual ~native_window() = default;

	// TODO: make private somehow?
	virtual void swap_frame_buffers() {}

	void set_fullscreen(bool enable);

	bool is_fullscreen() const noexcept
	{
		return this->is_fullscreen_v;
	}

	// TODO: add set_maximized(bool) & is_maximized()
	// TODO: add set_minimized(bool) & is_minimized()
	// TODO: add set_hidden(bool) & is_hidden()

	/**
	 * @brief Handler of window close event.
	 * Invoked when user tries to close the window, e.g. by clicking the window's close button.
	 */
	std::function<void()> close_handler;

	// ================
	// = mouse cursor =

	using cursor_id = decltype(cursor_stack)::iterator;

	cursor_id push_mouse_cursor(mouse_cursor cursor);
	void pop_mouse_cursor(cursor_id i);

	virtual void set_mouse_cursor_visible(bool visible) {}

	/**
	 * @brief Show/hide the virtual keyboard.
	 * On mobile platforms this function should show/hide the on-screen keyboard.
	 * On desktop platforms with physical keyboard this funtion should do nothing.
	 */
	virtual void set_virtual_keyboard_visible(bool visible) noexcept {}
};
} // namespace ruis::render
