/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include "list_widget.hpp"

using namespace ruis;

list_provider::list_provider(const utki::shared_ref<ruis::context>& context) :
	context(context)
{}

utki::shared_ref<widget> list_provider::get_highlighted_widget(size_t index)
{
	return this->get_widget(index);
}

void list_widget::notify_model_change()
{
	// Model change will cause remove/add child widgets to the list's container,
	// because list item widgets will have to be re-created.
	// So we need to make sure it is not done while container's list of children is locked.
	// For that we can do the handling of the model change on next main loop by posting to ui thread.
	auto self = utki::make_weak_from(*this);
	this->context.get().post_to_ui_thread([self]() {
		if (auto w = self.lock()) {
			w->handle_model_change();
		}
	});
}

list_widget::list_widget(
	const utki::shared_ref<ruis::context>& context, //
	parameters params
) :
	widget(context, {}, {}),
	params(std::move(params))
{}
