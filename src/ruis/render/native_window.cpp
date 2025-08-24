#include "native_window.hpp"

#include <utki/debug.hpp>

using namespace ruis;

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
