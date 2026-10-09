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
#include "../widget.hpp"

#include "overlay.hpp"

using namespace std::string_literals;

namespace {
namespace m = ruis::make;

/**
 * @brief Hover/press state of a context menu item.
 * Shared between the click_proxy handlers of the item.
 */
struct highlight_state {
	bool hovered = false;
	bool pressed = false;
};

/**
 * @brief A ruis::list_provider which decorates the widgets of another provider.
 * Each item widget of the content provider is wrapped with a ruis::click_proxy
 * which shows a background of color_highlight color while the item is pressed
 * and of color_secondary color while the item is hovered.
 * Consecutive items are separated by a thin line of color_secondary color.
 */
class decorated_provider : public ruis::list_provider
{
	utki::unique_ref<ruis::list_provider> content;

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
	) const;
};

decorated_provider::decorated_provider(
	const utki::shared_ref<ruis::context>& context, //
	utki::unique_ref<ruis::list_provider> content
) :
	ruis::list_provider(context), //
	content(std::move(content))
{}

size_t decorated_provider::count() const noexcept
{
	return this->content.get().count();
}

utki::shared_ref<ruis::widget> decorated_provider::get_widget(size_t index) const
{
	auto n = this->content.get().count();
	return this->wrap_item(
		this->content.get().get_widget(index), //
		index, //
		index + 1 == n
	);
}

utki::shared_ref<ruis::widget> decorated_provider::wrap_item(
	const utki::shared_ref<ruis::widget>& widget, //
	size_t index, //
	bool is_last
) const
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

	// Per-item hover/press state, shared between the click_proxy handlers
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
					// so its hovered state is kept up to date by the container,
					// use it to correct the hovered state when the item is unpressed
					state->hovered = cp.is_hovered();
					update_background();
				},
				.click_handler = [this, index](auto& cp) {
					if (this->on_item_click) {
						this->on_item_click(index);
					}
				}
			},
			.mouse_proxy_params{
				.hovered_change_handler = [state, update_background](auto& cp, auto pointer_id) {
					state->hovered = cp.is_hovered(pointer_id);
					update_background();
				}
			}
		}
	);
	// clang-format on

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
			std::move(click_proxy)
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
				// fill the undefined values of the user-provided frame parameters
				// with the style defaults
				auto& style = context.get().style();
				auto& rp = params.params.rectangle;
				for (auto& r : rp.specific.corner_radii) {
					if (r.get().is_undefined()) {
						r = style.get_len_gap();
					}
				}
				auto& borders = rp.padding.specific.borders;
				if (borders.left().get().is_undefined()) {
					borders.left() = style.get_len_border();
				}
				if (borders.top().get().is_undefined()) {
					borders.top() = style.get_len_gap();
				}
				if (borders.right().get().is_undefined()) {
					borders.right() = style.get_len_border();
				}
				if (borders.bottom().get().is_undefined()) {
					borders.bottom() = style.get_len_gap();
				}
				if (rp.specific.fill_color.get().is_undefined()) {
					rp.specific.fill_color = style.get_color_background();
				}
				if (rp.specific.stroke_width.get().is_undefined()) {
					rp.specific.stroke_width = style.get_len_border();
				}
				if (rp.specific.stroke_color.get().is_undefined()) {
					rp.specific.stroke_color = style.get_color_primary();
				}
				// the layout of the frame padding must stay pile
				// regardless of the user-provided parameters
				rp.padding.container.layout = ruis::layout::pile;

				// clang-format off
				return m::rectangle(context,
					{
						.layout_params{
							// max width: the frame fills the menu width when the menu is shown with a
							// concrete width, and wraps its content when shown with its natural size
							.dims = {ruis::dim::max, ruis::dim::min}
						},
						.widget{
							.id = "ruis_contextmenu_frame"s,
							.clip = true
						},
						.params{ rp }
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
