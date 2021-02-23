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
