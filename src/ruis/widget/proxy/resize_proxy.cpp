/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include "resize_proxy.hpp"

using namespace ruis;

resize_proxy::resize_proxy(utki::shared_ref<ruis::context> context, all_parameters params) :
	widget(std::move(context), widget::all_parameters{std::move(params.widget_params)})
{}

void resize_proxy::on_resize()
{
	this->widget::on_resize();

	if (this->resize_handler) {
		this->resize_handler(*this);
	}
}
