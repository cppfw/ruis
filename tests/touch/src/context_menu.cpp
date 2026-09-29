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

#include <ruis/util/length.hpp>
#include <ruis/widget/container.hpp>
#include <ruis/widget/group/overlay.hpp>
#include <ruis/widget/label/rectangle.hpp>
#include <ruis/widget/proxy/click_proxy.hpp>
#include <ruis/widget/proxy/mouse_proxy.hpp>
#include <ruis/widget/widget.hpp>
#include <utki/shared.hpp>

namespace context_menu {

namespace {
namespace m = ruis::make;

using ruis::length;
using ruis::real;
using ruis::vec2;

using std::max;
using std::min;

/**
 * @brief Hover/press state of a context menu item.
 * Shared between the mouse_proxy and click_proxy handlers of the item.
 */
struct highlight_state {
	bool hovered = false;
	bool pressed = false;
};
} // namespace

// ------------------------
// = context_menu widget =
// ------------------------

context_menu::context_menu(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters params
) :
	// NOTE: ruis::widget is a virtual base class of ruis::touch::list,
	//       so it has to be initialized here (by the most derived class).
	// clang-format off
	widget( //
		context, //
		std::move(params.layout_params), //
		[&]() {
			if (!params.widget.clip.has_value()) {
				params.widget.clip = true;
			}
			return std::move(params.widget);
		}()
	),
	// clang-format on
	ruis::touch::list(context, std::move(params))
{}

// ---------------------------
// = context_menu_provider =
// ---------------------------

context_menu_provider::context_menu_provider(
	const utki::shared_ref<ruis::context>& context, //
	ruis::widget_list widgets
) :
	ruis::list_provider(context)
{
	this->items.reserve(widgets.size());
	for (size_t i = 0; i != widgets.size(); ++i) {
		this->items.push_back(this->wrap_item(widgets[i], i + 1 == widgets.size()));
	}
}

size_t context_menu_provider::count() const noexcept
{
	return this->items.size();
}

utki::shared_ref<ruis::widget> context_menu_provider::get_widget(size_t index)
{
	return this->items[index];
}

utki::shared_ref<ruis::widget> context_menu_provider::wrap_item(
	const utki::shared_ref<ruis::widget>& content, //
	bool is_last
)
{
	auto& style = this->context.get().style();

	// background of the item, shown while the item is pressed or hovered
	// (in color_highlight or color_secondary color respectively)
	// clang-format off
	auto background = m::rectangle(this->context,
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
	// clang-format on

	auto* bg = &background.get();

	// Per-item hover/press state, shared between the mouse_proxy and click_proxy handlers
	auto state = std::make_shared<highlight_state>();

	// clang-format off
	auto update_background = [this, state, bg]() {
		auto& style = this->context.get().style();
		if (state->pressed) {
			bg->set_visible(true);
			bg->set_fill_color(style.get_color_highlight());
		} else if (state->hovered) {
			bg->set_visible(true);
			bg->set_fill_color(style.get_color_secondary());
		} else {
			bg->set_visible(false);
		}
	};
	// clang-format on

	// clang-format off
	auto click_proxy = m::click_proxy(this->context,
		{
			.layout_params{
				.dims = {ruis::dim::fill, ruis::dim::fill}
			},
			.click_proxy_params{
				.pressed_change_handler = [state, update_background](auto& cp) {
					state->pressed = cp.is_pressed();
					// while the button is down, the click_proxy captures the mouse,
					// so its hovered state is kept up to date by the container
					// (the mouse_proxy underneath is not), use it to correct
					// the hovered state when the item is unpressed
					state->hovered = cp.is_hovered();
					update_background();
				},
				.click_handler = [this](auto& cp) {
					if (this->on_item_click) {
						this->on_item_click();
					}
				}
			}
		}
	);
	// clang-format on

	// clang-format off
	auto mouse_proxy = m::mouse_proxy(this->context,
		{
			.layout_params{
				.dims = {ruis::dim::fill, ruis::dim::fill}
			},
			.mouse_proxy_params{
				.hovered_change_handler = [state, update_background](auto& mp, auto pointer_id) {
					state->hovered = mp.is_hovered(pointer_id);
					update_background();
				}
			}
		}
	);
	// clang-format on

	// The click_proxy catches all mouse events, the mouse_proxy is placed on top of it
	// so that it receives hover notifications.
	// clang-format off
	auto item = m::pile(this->context,
		{
			.layout_params{
				.dims = {ruis::dim::max, ruis::dim::min}
			}
		},
		{
			std::move(background),
			std::move(content),
			std::move(click_proxy),
			std::move(mouse_proxy)
		}
	);
	// clang-format on

	if (is_last) {
		return item;
	}

	// A separator between consecutive items
	// clang-format off
	auto separator = m::rectangle(this->context,
		{
			.layout_params{
				.dims = {ruis::dim::fill, style.get_len_border()}
			},
			.params{
				.specific{
					.fill_color = style.get_color_secondary()
				}
			}
		},
		{}
	);
	// clang-format on

	// clang-format off
	return m::column(this->context,
		{
			.layout_params{
				.dims = {ruis::dim::max, ruis::dim::min}
			}
		},
		{
			std::move(item),
			std::move(separator)
		}
	);
	// clang-format on
}

// -------------------------
// = show() utility func =
// -------------------------

namespace {

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
	ruis::widget_list widgets
)
{
	auto olay = anchor.try_get_ancestor<ruis::overlay>();
	if (!olay) {
		throw std::logic_error("context_menu::show(): no overlay ancestor found");
	}

	auto& context = anchor.context;
	auto& style = context.get().style();
	auto screen = olay->rect().d;

	// compute the natural menu width from the item widgets
	vec2 menu_size(0);
	for (auto& w : widgets) {
		auto d = ruis::dims_for_widget(w.get(), screen);
		menu_size.x() = max(menu_size.x(), d.x());
	}

	auto provider = utki::make_unique<context_menu_provider>(context, std::move(widgets));
	auto& provider_ref = provider.get();

	// compute the natural menu height from the wrapped items
	// (the wrapped items include the separators between the items)
	real menu_height = 0;
	for (size_t i = 0; i != provider.get().count(); ++i) {
		menu_height += ruis::dims_for_widget(provider.get().get_widget(i).get(), screen).y();
	}

	// the menu is min-wrap vertically, but clamped to fit on the screen
	// (minus the frame's top and bottom borders), so that long menus
	// are clamped to the screen size and can be scrolled instead
	real frame_v_border = style.get_len_gap().get().get(context) * 2;
	real list_height = max(real(0), min(menu_height, screen.y() - frame_v_border));

	// clang-format off
	auto menu_params = context_menu::all_parameters{
		.layout_params{
			.dims = {ruis::dim(length(menu_size.x())), ruis::dim(length(list_height))}
		},
		.params{
			.specific{
				.provider = std::move(provider)
			}
		}
	};
	// clang-format on
	auto menu = utki::make_shared<context_menu>(context, std::move(menu_params));

	// the frame: a rectangle with a border that wraps the menu
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
							style.get_len_border(), // left
							style.get_len_gap(), // top
							style.get_len_border(), // right
							style.get_len_gap() // bottom
						}
					}
				},
				.specific{
					.corner_radii = {style.get_len_gap()},
					.fill_color = style.get_color_background(),
					.stroke_width = style.get_len_border(),
					.stroke_color = style.get_color_primary()
				}
			}
		},
		{
			std::move(menu)
		}
	);
	// clang-format on

	auto anchor_pos = compute_anchor(anchor, *olay, frame.get());

	auto popup = olay->show_popup(frame, anchor_pos);
	auto popup_ref = utki::make_weak(popup);

	provider_ref.on_item_click = [popup_ref, context]() {
		close_popup(popup_ref, context);
	};
}

} // namespace context_menu
