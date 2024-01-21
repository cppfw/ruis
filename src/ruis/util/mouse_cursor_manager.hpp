/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#include "mouse_cursor.hpp"

namespace morda {

class mouse_cursor_manager
{
	std::list<mouse_cursor> cursor_stack = {mouse_cursor::arrow};

	const std::function<void(morda::mouse_cursor)> set_mouse_cursor;

public:
	using iterator = decltype(cursor_stack)::iterator;

	mouse_cursor_manager(std::function<void(morda::mouse_cursor)>&& set_mouse_cursor) :
		set_mouse_cursor(std::move(set_mouse_cursor))
	{}

	iterator push(mouse_cursor cursor);

	void pop(iterator i);
};

} // namespace morda
