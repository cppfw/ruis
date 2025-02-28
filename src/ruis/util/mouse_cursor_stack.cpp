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

#include "mouse_cursor_stack.hpp"

#include <utki/debug.hpp>

using namespace ruis;

mouse_cursor_stack::mouse_cursor_stack(std::function<void(ruis::mouse_cursor)> set_mouse_cursor) :
	set_mouse_cursor(std::move(set_mouse_cursor))
{
	if(!this->set_mouse_cursor){
		throw std::invalid_argument("mouse_cursor_stack::mouse_cursor_stack(): set_mouse_cursor function cannot be null");
	}
}

decltype(mouse_cursor_stack::cursor_stack)::iterator mouse_cursor_stack::push(mouse_cursor cursor)
{
	this->cursor_stack.push_front(cursor);

	this->set_mouse_cursor(cursor);

	return this->cursor_stack.begin();
}

void mouse_cursor_stack::pop(decltype(cursor_stack)::iterator i)
{
	ASSERT(this->cursor_stack.size() > 1)
	bool top_cursor = i == this->cursor_stack.begin();

	this->cursor_stack.erase(i);

	if (top_cursor) {
		// top cursor has just been removed, set the new top cursor
		this->set_mouse_cursor(this->cursor_stack.front());
	}
}
