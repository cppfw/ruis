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

#include "frame_widget.hpp"

using namespace std::string_view_literals;

using namespace ruis;

frame_widget::frame_widget(
	utki::shared_ref<ruis::context> context,
	container::parameters container_params,
	parameters frame_params,
	utki::span<const utki::shared_ref<widget>> frame_layout,
	utki::span<const utki::shared_ref<widget>> children
) :
	widget(std::move(context), {}, {}),
	container(//
		this->context,
		{
			.container_params = std::move(container_params)
		},
		frame_layout
	),
	params(std::move(frame_params)),
	inner_content(this->get_widget_as<container>("ruis_content"sv))
{
	this->inner_content.push_back(children);
}

void frame_widget::set_borders(sides<length> borders)
{
	this->params.borders = borders;
	this->on_borders_change();
}
