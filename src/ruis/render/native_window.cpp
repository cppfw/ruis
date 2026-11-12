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

#include "native_window.hpp"

#include <algorithm>
#include <iterator>

#include <utki/debug.hpp>

using namespace ruis::render;

void native_window::set_vsync_enabled(bool enabled)
{
	if (enabled == this->is_vsync_enabled()) {
		return;
	}

	this->set_vsync_enabled_internal(enabled);

	this->is_vsync_enabled_v = enabled;
}

void native_window::set_fullscreen(bool enable)
{
	if (enable == this->is_fullscreen()) {
		return;
	}

	this->set_fullscreen_internal(enable);

	this->is_fullscreen_v = enable;
}

native_window::cursor_id native_window::push_mouse_cursor(mouse_cursor cursor)
{
	auto id = this->next_cursor_id++;

	this->cursor_stack.push_front({id, cursor});

	this->set_mouse_cursor(cursor);

	return id;
}

void native_window::pop_mouse_cursor(cursor_id i)
{
	auto it = std::find_if(this->cursor_stack.begin(), this->cursor_stack.end(), [i](const cursor_stack_entry& e) {
		return e.id == i;
	});

	// the cursor was not pushed (or only the default bottom cursor is left), nothing to pop
	if (it == this->cursor_stack.end() || std::next(it) == this->cursor_stack.end()) {
		return;
	}

	bool top_cursor = it == this->cursor_stack.begin();

	this->cursor_stack.erase(it);

	if (top_cursor) {
		// top cursor has just been removed, set the new top cursor
		this->set_mouse_cursor(this->cursor_stack.front().cursor);
	}
}
