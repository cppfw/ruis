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

#include "context.hpp"

#include "widget.hpp"

using namespace morda;

context::context(
		const utki::shared_ref<morda::renderer>& r,
		const utki::shared_ref<morda::updater>& u,
		std::function<void(std::function<void()>&&)>&& run_from_ui_thread_function,
		std::function<void(morda::mouse_cursor)>&& set_mouse_cursor_function,
		real dots_per_inch,
		real dots_per_dp
	) :
		renderer(r),
		updater(u),
		run_from_ui_thread(std::move(run_from_ui_thread_function)),
		cursor_manager(std::move(set_mouse_cursor_function)),
		loader(*this),
		inflater(*this),
		units(dots_per_inch, dots_per_dp)
{
	if(!this->run_from_ui_thread){
		throw std::invalid_argument("context::context(): no post to UI thread function provided");
	}
}

void context::set_focused_widget(const std::shared_ptr<widget>& w){
	if(auto prev = this->focused_widget.lock()){
		prev->focused = false;
		prev->on_focus_change();
	}

	this->focused_widget = w;

	if(w){
		w->focused = true;
		w->on_focus_change();
	}
}
