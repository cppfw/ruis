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

#include <ruis/util/length.hpp>
#include <ruis/widget/container.hpp>
#include <ruis/widget/label/padding.hpp>
#include <ruis/widget/label/rectangle.hpp>
#include <ruis/widget/label/text.hpp>
#include <ruis/widget/widget.hpp>
#include <utki/shared.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace context_menu {

namespace {
namespace m = ruis::make;

using ruis::length;
using ruis::vec2;

/**
 * @brief A constructed context menu action item.
 * Holds the item widget along with direct references to the parts
 * that need to be wired up after the menu is shown.
 */
struct action_item {
	utki::shared_ref<ruis::widget> widget;
	ruis::mouse_proxy& mouse_proxy;
	ruis::rectangle& highlight;
};

action_item make_action_item(
	const utki::shared_ref<ruis::context>& context, //
	const ruis::string& label
)
{
	auto& style = context.get().style();

	const auto h_pad = length::make_pp(12);
	const auto v_pad = length::make_pp(6);

	// clang-format off
	auto highlight = m::rectangle(context,
		{
			.layout_params{
				.dims = {ruis::dim::fill, ruis::dim::fill}
			},
			.widget{
				.visible = false
			},
			.params{
				.specific{
					.fill_color = style.get_color_highlight()
				}
			}
		},
		{}
	);

	auto content = m::padding(context,
		{
			.layout_params{
				.dims = {ruis::dim::min, ruis::dim::min}
			},
			.params{
				.container{
					.layout = ruis::layout::pile
				},
				.specific{
					.borders = {h_pad, v_pad, h_pad, v_pad}
				}
			}
		},
		{
			m::text(context,
				{
					.layout_params{
						.dims = {ruis::dim::min, ruis::dim::min}
					}
				},
				label
			)
		}
	);

	auto mouse_proxy = m::mouse_proxy(context,
		{
			.layout_params{
				.dims = {ruis::dim::fill, ruis::dim::fill}
			}
		}
	);

	// keep raw pointers to the parts we need to wire up after the menu is shown
	auto* mp = &mouse_proxy.get();
	auto* hl = &highlight.get();

	auto item = m::pile(context,
		{
			.layout_params{
				.dims = {ruis::dim::max, ruis::dim::min}
			}
		},
		{
			std::move(highlight),
			std::move(content),
			std::move(mouse_proxy)
		}
	);
	// clang-format on

	return action_item{
		std::move(item), //
		*mp, //
		*hl
	};
}

// clang-format off
utki::shared_ref<ruis::widget> make_separator(
	const utki::shared_ref<ruis::context>& context
)
{
	return m::rectangle(context,
		{
			.layout_params{
				.dims = {ruis::dim::fill, context.get().style().get_len_border()}
			},
			.params{
				.specific{
					.fill_color = context.get().style().get_color_secondary()
				}
			}
		},
		{}
	);
}

// clang-format on

void close_popup(
	const std::weak_ptr<ruis::widget>& popup, //
	const utki::shared_ref<ruis::context>& context
)
{
	if (auto p = popup.lock()) {
		context.get().post_to_ui_thread([p]() {
			if (p->parent()) {
				p->remove_from_parent();
			}
		});
	}
}

vec2 compute_anchor(
	ruis::widget& anchor, //
	const ruis::overlay& olay, //
	const ruis::widget& menu
)
{
	auto btn_pos = anchor.get_pos_in_ancestor(vec2(0), &olay);
	auto btn_size = anchor.rect().d;
	auto screen = olay.rect().d;

	// natural menu size, clamped to the screen the same way show_popup() does
	auto menu_size = ruis::dims_for_widget(menu, screen);
	menu_size = min(menu_size, screen);

	// place the menu right below the anchor button, right-aligned with it
	// (show_popup() will clamp the position to keep the menu on the screen)
	vec2 pos;
	pos.x() = btn_pos.x() + btn_size.x() - menu_size.x();
	pos.y() = btn_pos.y() + btn_size.y();

	return pos;
}

} // namespace

void show(
	ruis::widget& anchor, //
	std::vector<action> actions
)
{
	auto olay = anchor.try_get_ancestor<ruis::overlay>();
	if (!olay) {
		throw std::logic_error("context_menu::show(): no overlay ancestor found");
	}

	auto& context = anchor.context;

	// build the vertical list of actions (a plain ruis::column, not a ruis::list)
	// clang-format off
	auto column = m::column(context,
		{
			.layout_params{
				.dims = {ruis::dim::max, ruis::dim::min}
			}
		}
	);
	// clang-format on

	std::vector<action_item> items;
	items.reserve(actions.size());
	for (size_t i = 0; i != actions.size(); ++i) {
		if (i != 0) {
			// a horizontal separator between consecutive actions
			column.get().push_back(make_separator(context));
		}
		auto item = make_action_item(context, actions[i].label);
		column.get().push_back(std::move(item.widget));
		items.push_back(std::move(item));
	}

	// the frame: a rectangle with a border that wraps the actions column
	// clang-format off
	auto frame = m::rectangle(context,
		{
			.layout_params{
				.dims = {ruis::dim::min, ruis::dim::min}
			},
			.widget{
				.clip = true
			},
			.params{
				.padding{
					.container{
						.layout = ruis::layout::pile
					},
					.specific{
						.borders = {
							context.get().style().get_len_border(), // left
							context.get().style().get_len_gap(), // top
							context.get().style().get_len_border(), // right
							context.get().style().get_len_gap() // bottom
						}
					}
				},
				.specific{
					.corner_radii = {context.get().style().get_len_gap()},
					.fill_color = context.get().style().get_color_background(),
					.stroke_width = context.get().style().get_len_border(),
					.stroke_color = context.get().style().get_color_primary()
				}
			}
		},
		{
			std::move(column)
		}
	);
	// clang-format on

	auto anchor_pos = compute_anchor(anchor, *olay, frame.get());

	auto popup = olay->show_popup(frame, anchor_pos);
	auto popup_ref = utki::make_weak(popup);

	// wire up the action items now that the popup is known
	for (size_t i = 0; i != items.size(); ++i) {
		auto& item = items[i];
		auto on_click = std::move(actions[i].on_click);

		item.mouse_proxy.hovered_change_handler = [hl = &item.highlight](ruis::mouse_proxy& w, unsigned pointer_id) {
			hl->set_visible(w.is_hovered(pointer_id));
		};

		item.mouse_proxy.mouse_button_handler =
			[on_click, popup_ref, context](ruis::mouse_proxy& w, const ruis::mouse_button_event& e) {
				if (e.action == ruis::button_action::release && w.is_hovered(e.pointer_id)) {
					if (on_click) {
						on_click();
					}
					close_popup(popup_ref, context);
				}
				return ruis::event_status::consumed;
			};
	}
}

} // namespace context_menu
