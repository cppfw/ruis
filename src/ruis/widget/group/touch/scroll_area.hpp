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

#pragma once

#include "../../base/touch/flickable.hpp"
#include "../scroll_area.hpp"

namespace ruis::touch {

class scroll_area :
	public ruis::scroll_area, //
	private touch::flickable
{
public:
	scroll_area(
		utki::shared_ref<ruis::context> context, //
		scroll_area::all_parameters params,
		widget_list children
	);

	event_status on_mouse_button(const mouse_button_event& event) override;
	event_status on_mouse_move(const mouse_move_event& event) override;

private:
	event_status flickable_on_mouse_button(const mouse_button_event& event) override;
	event_status flickable_on_mouse_move(const mouse_move_event& event) override;
	void flickable_scroll_by(const vec2& delta) override;
};

} // namespace ruis::touch
