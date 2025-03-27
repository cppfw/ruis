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

#include "context.hpp"

#include "widget/widget.hpp"

using namespace ruis;

context::context(
	utki::shared_ref<ruis::resource_loader> res_loader,
	utki::shared_ref<ruis::updater> updater,
	parameters params
) :
	post_to_ui_thread_function(std::move(params.post_to_ui_thread_function)),
	res_loader(std::move(res_loader)),
	renderer(this->res_loader.get().renderer),
	updater(std::move(updater)),
	cursor_stack(std::move(params.set_mouse_cursor_function)),
	localization(std::move(params.localization)),
	// style(this->res_loader),
	units(std::move(params.units))
{
	if (!this->post_to_ui_thread_function) {
		throw std::invalid_argument("context::context(): no post to UI thread function provided");
	}
}

void context::post_to_ui_thread(std::function<void()> proc)
{
	ASSERT(this->post_to_ui_thread_function)
	this->post_to_ui_thread_function(std::move(proc));
}

void context::set_focused_widget(const std::shared_ptr<widget>& w)
{
	if (auto prev = this->focused_widget.lock()) {
		prev->focused = false;
		prev->on_focus_change();
	}

	this->focused_widget = w;

	if (w) {
		w->focused = true;
		w->on_focus_change();
	}
}
