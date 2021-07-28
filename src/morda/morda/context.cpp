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

#include "context.hpp"

using namespace morda;


context::context(
		std::shared_ptr<morda::renderer> r,
		std::shared_ptr<morda::updater> u,
		std::function<void(std::function<void()>&&)>&& run_from_ui_thread_function,
		std::function<void(morda::mouse_cursor)>&& set_mouse_cursor_function,
		real dots_per_inch,
		real dots_per_dp
	) :
		renderer(std::move(r)),
		updater(std::move(u)),
		run_from_ui_thread(std::move(run_from_ui_thread_function)),
		cursor_manager(std::move(set_mouse_cursor_function)),
		loader(*this),
		inflater(*this),
		units(dots_per_inch, dots_per_dp)
{
	if(!this->renderer){
		throw std::invalid_argument("context::context(): passed in renderer pointer is null");
	}
	if(!this->run_from_ui_thread){
		throw std::invalid_argument("context::context(): no post to UI thread function provided");
	}
}

void context::set_focused_widget(std::shared_ptr<widget> w){
	if(auto prev = this->focused_widget.lock()){
		prev->isFocused_v = false;
		prev->on_focus_change();
	}

	this->focused_widget = w;

	if(w){
		w->isFocused_v = true;
		w->on_focus_change();
	}
}
