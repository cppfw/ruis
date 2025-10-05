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

#include "native_window.hpp"

#include <utki/debug.hpp>

using namespace ruis::render;

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
	this->cursor_stack.push_front(cursor);

	this->set_mouse_cursor(cursor);

	return this->cursor_stack.begin();
}

void native_window::pop_mouse_cursor(cursor_id i)
{
	utki::assert(this->cursor_stack.size() > 1, SL);
	bool top_cursor = i == this->cursor_stack.begin();

	this->cursor_stack.erase(i);

	if (top_cursor) {
		// top cursor has just been removed, set the new top cursor
		this->set_mouse_cursor(this->cursor_stack.front());
	}
}
