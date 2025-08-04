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

#include "list_widget.hpp"

using namespace ruis;

list_provider::list_provider(utki::shared_ref<ruis::context> context) :
	context(std::move(context))
{}

void list_provider::notify_model_change()
{
	if (!this->owner) {
		return;
	}

	this->context.get().post_to_ui_thread([owner = utki::make_weak_from(*this->owner)]() {
		if (auto o = owner.lock()) {
			o->handle_model_change();
		}
	});
}

list_widget::list_widget(
	utki::shared_ref<ruis::context> context, //
	parameters params
) :
	widget(std::move(context), {}, {}),
	params(std::move(params))
{
	this->params.provider.get().owner = this;
}

void list_widget::set_provider(utki::shared_ref<list_provider> provider)
{
	this->params.provider.get().owner = nullptr;

	this->params.provider = std::move(provider);

	this->params.provider.get().owner = this;

	this->handle_model_change();
}
