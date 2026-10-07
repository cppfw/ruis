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
#include "../../util/widget_list.hpp"
#include "../label/rectangle.hpp"
#include "../proxy/click_proxy.hpp"
#include "../proxy/mouse_proxy.hpp"
#include "../widget.hpp"

#include "overlay.hpp"

namespace {
namespace m = ruis::make;

/**
 * @brief Hover/press state of a context menu item.
 * Shared between the mouse_proxy and click_proxy handlers of the item.
 */
struct highlight_state {
	bool hovered = false;
	bool pressed = false;
};

/**
 * @brief A ruis::list_provider which decorates the widgets of another provider.
 * Each item widget of the content provider is wrapped with a ruis::click_proxy
 * and a ruis::mouse_proxy which show a background of color_highlight color while
 * the item is pressed and of color_secondary color while the item is hovered.
 * Consecutive items are separated by a thin line of color_secondary color.
 */
class decorated_provider : public ruis::list_provider
{
	utki::unique_ref<ruis::list_provider> content;
	ruis::widget_list items;

public:
	/**
	 * @brief Item click handler.
	 * Invoked when a menu item is clicked.
	 * The context_menu widget sets this handler to close the menu.
	 */
	std::function<void(size_t index)> on_item_click;

	decorated_provider(
		const utki::shared_ref<ruis::context>& context, //
		utki::unique_ref<ruis::list_provider> content
	);

	size_t count() const noexcept override;
	utki::shared_ref<ruis::widget> get_widget(size_t index) const override;

private:
	utki::shared_ref<ruis::widget> wrap_item(
		const utki::shared_ref<ruis::widget>& widget, //
		size_t index, //
		bool is_last
	);
};

decorated_provider::decorated_provider(
	const utki::shared_ref<ruis::context>& context, //
	utki::unique_ref<ruis::list_provider> content
) :
	ruis::list_provider(context), //
	content(std::move(content))
{
	auto& c = this->content.get();
	auto n = c.count();
	this->items.reserve(n);
	// TODONEXT: do not store widgets beforehand, create them right in get_widget().
	for (size_t i = 0; i != n; ++i) {
		this->items.push_back(this->wrap_item(c.get_widget(i), i, i + 1 == n));
	}
}

size_t decorated_provider::count() const noexcept
{
	return this->items.size();
}

utki::shared_ref<ruis::widget> decorated_provider::get_widget(size_t index) const
{
	return this->items[index];
}

utki::shared_ref<ruis::widget> decorated_provider::wrap_item(
	const utki::shared_ref<ruis::widget>& widget, //
	size_t index, //
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
				.click_handler = [this, index](auto& cp) {
					if (this->on_item_click) {
						this->on_item_click(index);
					}
				}
			}
		}
	);
	// clang-format on

	// TODONEXT: remove the mouse_proxy from here when click_proxy inherits mouse_proxy and handles hover state itself
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
			std::move(widget),
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

ruis::context_menu::context_menu(
	const utki::shared_ref<ruis::context>& context, //
	all_parameters params, //
	std::function<utki::shared_ref<ruis::list_widget>(ruis::list_widget::parameters)> list_factory
) :
	widget( //
		context, //
		std::move(params.layout_params), //
		std::move(params.widget) //
	),
	// clang-format off
	container( //
		context, //
		{ //
			.params{ //
				.layout = ruis::layout::pile //
			} //
		}, //
		{
			[&]() {
				// clang-format off
				// the decorated provider which wraps the provider given in the
				// 'list' parameter of params (see the class description)
				auto menu_provider = utki::make_unique<decorated_provider>(
					context, //
					std::move(params.params.list.provider)
				);
				menu_provider.get().on_item_click = [this](size_t index) {
					if (this->on_item_click) {
						this->on_item_click(index);
					}
					this->close();
				};

				auto list_params = ruis::list_widget::parameters{
					std::move(menu_provider)
				};
				auto list = list_factory(std::move(list_params));

				// the frame: a rectangle with a border that wraps the menu
				return m::rectangle(context,
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
					{ std::move(list) }
				);
			// clang-format on
		}()}
	)
// clang-format on
{}

void ruis::context_menu::close()
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

utki::shared_ref<ruis::widget> ruis::show_context_menu(
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
