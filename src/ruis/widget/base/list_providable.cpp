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

#include "list_providable.hpp"

using namespace ruis;

list_provider::list_provider(utki::shared_ref<ruis::context> context) :
	context(std::move(context))
{}

void list_provider::notify_model_change()
{
	this->context.get().post_to_ui_thread([this]() {
		this->model_change_signal.emit();
	});
}

list_providable::list_providable(parameters params)
{
	this->set_provider(params.provider);
}

void list_providable::set_provider(std::shared_ptr<list_provider> provider)
{
	if (this->params.provider) {
		this->params.provider->model_change_signal.disconnect(this->provider_signal_connection);
	}
	this->params.provider = std::move(provider);
	if (this->params.provider) {
		this->provider_signal_connection = //
			this->params.provider->model_change_signal.connect( //
				[this]() {
					this->handle_model_change();
				}
			);
	}
	// set_provider is called from constructor in order to connect
	// model change observer
	// NOLINTNEXTLINE(clang-analyzer-optin.cplusplus.VirtualCall)
	this->handle_model_change();
}
