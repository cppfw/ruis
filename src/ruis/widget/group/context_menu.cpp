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

#include "context_menu.hpp"

#include <algorithm>

#include <utki/shared.hpp>

#include "../../context.hpp"
#include "../widget.hpp"

#include "overlay.hpp"

namespace {

ruis::vec2 compute_anchor(
	ruis::widget& anchor, //
	const ruis::overlay& olay, //
	const ruis::widget& menu
)
{
	auto btn_pos = anchor.get_pos_in_ancestor(ruis::vec2(0), &olay);
	auto btn_size = anchor.rect().d;
	auto screen = olay.rect().d;

	// natural menu size, clamped to the screen the same way show_popup() does
	auto menu_size = menu.measure_within_parent(
		screen, //
		r4::vector2<ruis::measure_mode>(ruis::measure_mode::exactly)
	);
	menu_size = std::min(menu_size, screen);

	// place the menu right below the anchor, right-aligned with it
	// (show_popup() will clamp the position to keep the menu on the screen)
	ruis::vec2 pos;
	pos.x() = btn_pos.x() + btn_size.x() - menu_size.x();
	pos.y() = btn_pos.y() + btn_size.y();

	return pos;
}

} // namespace

namespace ruis {

context_menu::context_menu(
	const utki::shared_ref<ruis::context>& context, //
	ruis::layout_parameters layout_params, //
	ruis::widget::parameters widget_params
) :
	widget(
		context, //
		std::move(layout_params),
		std::move(widget_params)
	)
{}

void context_menu::close()
{
	auto self_weak = utki::make_weak_from(*this);
	this->context.get().post_to_ui_thread([self_weak]() {
		if (auto self = self_weak.lock()) {
			if (self->parent()) {
				self->remove_from_parent();
			}
		}
	});
}

utki::shared_ref<ruis::widget> show_context_menu(
	ruis::widget& anchor, //
	utki::shared_ref<ruis::widget> menu
)
{
	auto olay = anchor.try_get_ancestor<ruis::overlay>();
	if (!olay) {
		throw std::logic_error("show_context_menu(): no overlay ancestor found");
	}

	auto pos = compute_anchor(anchor, *olay, menu.get());
	return olay->show_popup(std::move(menu), pos);
}

} // namespace ruis
