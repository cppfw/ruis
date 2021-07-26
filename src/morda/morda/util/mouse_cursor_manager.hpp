/*
morda GUI framework
Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

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

#include <list>
#include <functional>

#include "mouse_cursor.hpp"

namespace morda{

class mouse_cursor_manager{
	std::list<mouse_cursor> cursor_stack = {mouse_cursor::arrow};

	const std::function<void(morda::mouse_cursor)> set_mouse_cursor;
public:
	mouse_cursor_manager(std::function<void(morda::mouse_cursor)>&& set_mouse_cursor) :
			set_mouse_cursor(std::move(set_mouse_cursor))
	{}

	decltype(cursor_stack)::iterator push(mouse_cursor cursor);

	void pop(decltype(cursor_stack)::iterator i);
};

}
