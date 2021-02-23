#include "mouse_cursor_manager.hpp"

#include <utki/debug.hpp>

using namespace morda;

decltype(mouse_cursor_manager::cursor_stack)::iterator mouse_cursor_manager::push(mouse_cursor cursor){
	this->cursor_stack.push_front(cursor);

	this->set_mouse_cursor(cursor);

	return this->cursor_stack.begin();
}

void mouse_cursor_manager::pop(decltype(cursor_stack)::iterator i){
	ASSERT(this->cursor_stack.size() > 1)
	bool top_cursor = i == this->cursor_stack.begin();
	
	this->cursor_stack.erase(i);

	if(top_cursor){
		// top cursor has just been removed, set the new top cursor
		this->set_mouse_cursor(this->cursor_stack.front());
	}
}
