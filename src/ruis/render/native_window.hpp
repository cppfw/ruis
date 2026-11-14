/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include <r4/vector.hpp>

#include "../util/keyboard_kind.hpp"
#include "../util/mouse_cursor.hpp"

namespace ruis::render {
class context;

/**
 * @brief A native OS window.
 * This is an abstract base class representing a window created by the
 * underlying operating system. Concrete render backends provide
 * implementations which bind a rendering context to the window.
 */
class native_window
{
	friend class ruis::render::context;

	bool is_vsync_enabled_v = false;
	bool is_fullscreen_v = false;
	bool is_maximized_v = false;
	bool is_minimized_v = false;

	/**
	 * @brief Bind rendering context to the native window.
	 */
	virtual void bind_rendering_context() {}

	/**
	 * @brief Check if rendering context is bound to the native window.
	 * @return true if rendering context is bound.
	 */
	virtual bool is_rendering_context_bound() const noexcept
	{
		return false;
	}

	/**
	 * @brief Set fullscreen mode internally.
	 * Not intended to be called by the user directly.
	 * @param enable - true to enable fullscreen mode, false to disable it.
	 */
	virtual void set_fullscreen_internal(bool enable) {}

	/**
	 * @brief Set vsync enabled state internally.
	 * Not intended to be called by the user directly.
	 * @param enabled - true to enable vsync, false to disable it.
	 */
	virtual void set_vsync_enabled_internal(bool enabled) {}

	/**
	 * @brief Set vsync enabled state.
	 * @param enabled - true to enable vsync, false to disable it.
	 */
	void set_vsync_enabled(bool enabled);

	/**
	 * @brief A single entry in the mouse cursor stack.
	 */
	struct cursor_stack_entry {
		/**
		 * @brief Unique id of the pushed cursor.
		 */
		unsigned id;

		/**
		 * @brief The pushed mouse cursor.
		 */
		mouse_cursor cursor;
	};

	// the bottom (0) entry is the default cursor, it is never popped
	std::list<cursor_stack_entry> cursor_stack = {
		{.id = 0, .cursor = mouse_cursor::arrow}
	};
	unsigned next_cursor_id = 1;

	/**
	 * @brief Set mouse cursor.
	 * @param c - mouse cursor to set.
	 */
	virtual void set_mouse_cursor(ruis::mouse_cursor c) {}

protected:
	/**
	 * @brief Check if vsync is enabled.
	 * @return true if vsync is enabled.
	 */
	bool is_vsync_enabled() const noexcept
	{
		return this->is_vsync_enabled_v;
	}

public:
	/**
	 * @brief Default constructor.
	 */
	native_window() = default;

	native_window(const native_window&) = delete;
	native_window& operator=(const native_window&) = delete;

	native_window(native_window&&) = delete;
	native_window& operator=(native_window&&) = delete;

	/**
	 * @brief Default destructor.
	 */
	virtual ~native_window() = default;

	// ========================
	// = content presentation =

	/**
	 * @brief Swap frame buffers, presenting the back buffer to the screen.
	 */
	virtual void swap_frame_buffers() {}

	// =====================
	// = window dimensions =

	/**
	 * @brief Set fullscreen mode.
	 * @param enable - true to enable fullscreen mode, false to disable it.
	 */
	void set_fullscreen(bool enable);

	/**
	 * @brief Check if the window is in fullscreen mode.
	 * @return true if the window is in fullscreen mode.
	 */
	bool is_fullscreen() const noexcept
	{
		return this->is_fullscreen_v;
	}

	/**
	 * @brief Maximize or unmaximize the window.
	 * @param maximized - true to maximize the window, false to unmaximize it.
	 */
	virtual void set_maximized(bool maximized) noexcept {}

	/**
	 * @brief Check if the window is maximized.
	 * @return true if the window is maximized.
	 */
	bool is_maximized() const noexcept
	{
		return this->is_maximized_v;
	}

	/**
	 * @brief Minimize or unminimize the window.
	 * @param minimized - true to minimize the window, false to unminimize it.
	 */
	virtual void set_minimized(bool minimized) noexcept {}

	/**
	 * @brief Check if the window is minimized.
	 * @return true if the window is minimized.
	 */
	bool is_minimized() const noexcept
	{
		return this->is_minimized_v;
	}

	/**
	 * @brief Get window dimensions.
	 * @return window dimensions in physical pixels.
	 */
	virtual r4::vector2<unsigned> get_dims() const noexcept
	{
		return {0, 0};
	}

	/**
	 * @brief Set window dimensions.
	 * Does not guarantee exactly the requested dims will be set.
	 * @param dims - window dimensions to set in physical pixels.
	 */
	virtual void set_dims(const r4::vector2<unsigned> dims) const noexcept {}

	// ================
	// = mouse cursor =

	// a cursor_id is a unique id of a pushed cursor, it is used to pop it later.
	// 0 means the cursor was not pushed.
	/**
	 * @brief Type of a pushed mouse cursor id.
	 */
	using cursor_id = unsigned;

	/**
	 * @brief Push a mouse cursor onto the cursor stack, making it the current cursor.
	 * @param cursor - mouse cursor to push.
	 * @return unique id of the pushed cursor, to be used to pop it later.
	 */
	cursor_id push_mouse_cursor(mouse_cursor cursor);

	/**
	 * @brief Pop a mouse cursor from the cursor stack by its id.
	 * @param i - unique id of the mouse cursor to pop.
	 */
	void pop_mouse_cursor(cursor_id i);

	/**
	 * @brief Set mouse cursor visibility.
	 * @param visible - true to make the mouse cursor visible, false to hide it.
	 */
	virtual void set_mouse_cursor_visible(bool visible) {}

	// =========
	// = other =

	/**
	 * @brief Show/hide the virtual keyboard.
	 * On mobile platforms this function should show/hide the on-screen keyboard.
	 * On desktop platforms with physical keyboard this funtion should do nothing.
	 * @param visible - true to show the virtual keyboard, false to hide it.
	 * @param kind - the kind of the keyboard to show (only meaningful when `visible` is true).
	 */
	virtual void set_virtual_keyboard_visible(bool visible, ruis::keyboard_kind kind) noexcept {}

	/**
	 * @brief Handler of window close event.
	 * Invoked when user tries to close the window, e.g. by clicking the window's close button.
	 */
	std::function<void()> close_handler;
};
} // namespace ruis::render
