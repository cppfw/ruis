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

#include "selection_box.hpp"

#include "../../group/overlay.hpp"
#include "../../group/touch/list.hpp"
#include "../../label/gap.hpp"
#include "../../label/padding.hpp"
#include "../../label/rectangle.hpp"
#include "../../label/text.hpp"
#include "../../proxy/click_proxy.hpp"
#include "../../proxy/mouse_proxy.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;
using namespace ruis::length_literals;

using namespace ruis::touch;

namespace {
constexpr auto selection_label_id = "ruis_touch_selection_box_selection"sv;
constexpr auto default_padding = 10_pp;
} // namespace

selection_box::selection_box(
	utki::shared_ref<ruis::context> context, //
	all_parameters params
) :
	widget(
		std::move(context), //
		std::move(params.layout_params),
		std::move(params.widget_params)
	),
	button(
		this->context,
		{
}
	),
	push_button(this->context, {}),
	// clang-format off
	rectangle_push_button(this->context,
		{
			.container_params{
				.layout = ruis::layout::row
			},
			.padding_params{
				.borders = {default_padding}
			},
			.rectangle_params{
				.corner_radii = {default_padding}
			}
		},
		{
			ruis::make::text(this->context,
				{
					.layout_params{
						.align = {ruis::align::front, ruis::align::center}
					},
					.color_params{
						.color = this->context.get().style().get_color_text_normal()
					}
				},
				std::move(params.title)
			),
			ruis::make::container(this->context,
				{
					.layout_params{
						.weight = 1,
						.align = {ruis::align::back, ruis::align::center}
					},
					.widget_params{
						// TODO: refactor to avoid using lookup widget by id
						.id = std::string(selection_label_id)
					},
					.container_params{
						.layout = ruis::layout::pile
					}
				}
			)
		}
	),
	// clang-format on
	ruis::selection_box(
		this->context, //
		this->get_widget_as<ruis::container>(selection_label_id),
		std::move(params.list_params)
	)
{}

void selection_box::on_click()
{
	this->show_selection_menu();
}

namespace ruis::touch {
class selection_box::wrapping_provider : public ruis::list_provider
{
	// Store strong reference to owner selection_box to kae sure it is alive
	// while selection menu is shown.
	utki::shared_ref<ruis::touch::selection_box> owner;

public:
	wrapping_provider(utki::shared_ref<ruis::touch::selection_box> owner) :
		ruis::list_provider(owner.get().context),
		owner(std::move(owner))
	{}

	size_t count() const noexcept override
	{
		return this->owner.get().get_provider().count();
	}

	utki::shared_ref<ruis::widget> get_widget(size_t index) override
	{
		// clang-format off
		auto pressed_indicator = ruis::make::rectangle(this->context,
			{
				.layout_params{
					.dims = {ruis::dim::fill, ruis::dim::fill}
				},
				.widget_params{
					.visible = false
				},
				.color_params{
					.color = this->context.get().style().get_color_highlight()
				}
			}
		);
		// clang-format on

		// clang-format off
		auto click_proxy = ruis::make::click_proxy(this->context,
			{
				.layout_params{
					.dims = {ruis::dim::fill, ruis::dim::fill}
				},
				.click_proxy_params{
					.pressed_change_handler = [&pi = pressed_indicator.get()](auto& cp){
						// std::cout << "pressed = " << cp.is_pressed() << std::endl;
						pi.set_visible(cp.is_pressed());
					},
					.click_handler = [owner = this->owner, index](auto& cp){
						owner.get().close_selection_menu();
						if(owner.get().get_selection() != index){
							owner.get().set_selection(index);
							owner.get().notify_selection_changed();
						}
					}
				}
			}
		);
		// clang-format on

		// clang-format off
		return ruis::make::column(this->context,
			{
				.layout_params{
					.dims = {ruis::dim::fill, ruis::dim::min}
				}
			},
			{
				ruis::make::pile(this->context,
					{
						.layout_params{
							.dims = {ruis::dim::fill, ruis::dim::min}
						}
					},
					{
						std::move(pressed_indicator),
						ruis::make::padding(this->context,
							{
								.layout_params{
									.dims = {ruis::dim::fill, ruis::dim::min}
								},
								.container_params{
									.layout = ruis::layout::pile
								},
								.padding_params{
									.borders = {default_padding}
								}
							},
							{
								[&]() {
									if (index == this->owner.get().get_selection()) {
										return this->owner.get().get_provider().get_highlighted_widget(index);
									} else {
										return this->owner.get().get_provider().get_widget(index);
									}
								}()
							}
						),
						std::move(click_proxy)
					}
				),
				ruis::make::gap(this->context,
					{
						.layout_params{
							.dims = {ruis::dim::fill, ruis::length::make_pp(1)}
						},
						.color_params{
							.color = this->context.get().style().get_color_separator()
						}
					}
				)
			}
		);
		// clang-format on
	}
};
} // namespace ruis::touch

void selection_box::show_selection_menu()
{
	auto& olay = this->get_ancestor<overlay>();

	auto& c = this->context;

	// clang-format off
	auto bg_click_proxy = ruis::make::click_proxy(c,
		{
			.layout_params{
				.dims = {ruis::dim::fill, ruis::dim::fill}
			}
		}
	);
	// clang-format on

	// clang-format off
	auto root = ruis::make::pile(c,
		{
			.layout_params{
				.dims = {ruis::dim::fill, ruis::dim::fill}
			}
		},
		{
			ruis::make::rectangle(c,
				{
					.layout_params{
						.dims = {ruis::dim::fill, ruis::dim::fill}
					},
					.widget_params{
						.rectangle = {0, 0, 0, 0}
					},
					.color_params{
						.color = this->context.get().style().get_color_dimmed()
					}
				}
			),
			bg_click_proxy,
			ruis::make::padding(c,
				{
					.layout_params{
						.dims = {ruis::dim::fill, ruis::dim::fill}
					},
					.padding_params{
						.borders = {30_pp}
					}
				},
				{
					// mouse proxy to consume mouse events, to prevent the menu from closing if clicked in the rectangle area
					ruis::make::mouse_proxy(c,
						{
							.layout_params{
								.dims = {ruis::dim::fill, ruis::dim::fill}
							},
							.mouse_proxy_params{
								.mouse_button_handler = [](auto&, auto&){return ruis::event_status::consumed;},
								.mouse_move_handler = [](auto&, auto&){return ruis::event_status::consumed;}
							}
						}
					),
					ruis::make::rectangle(c,
						{
							.layout_params{
								.dims = {ruis::dim::fill, ruis::dim::fill}
							},
							.padding_params{
								.borders = {20_pp} // TODO: get from theme
							},
							.color_params{
								.color = c.get().style().get_color_foreground()
							},
							.rectangle_params{
								.corner_radii = {20_pp} // TODO: get from theme
							}
						},
						{
							ruis::touch::make::list(c,
								{
									.layout_params{
										.dims = {ruis::dim::fill, ruis::dim::fill}
									},
									.list_params{
										.provider = utki::make_shared<wrapping_provider>(utki::make_shared_from(*this))
									}
								}
							)
						}
					)
				}
			)
		}
	);
	// clang-format on

	bg_click_proxy.get().click_handler = [weak_root = utki::make_weak(root)](ruis::click_proxy& cp) {
		// std::cout << "clicked" << std::endl;
		if (auto r = weak_root.lock()) {
			r->context.get().post_to_ui_thread([r]() {
				r->remove_from_parent();
			});
		}
	};

	c.get().post_to_ui_thread([olay = utki::make_shared_from(olay), root]() {
		olay.get().push_back(root);
	});

	this->selection_menu = root.to_shared_ptr();
}

void selection_box::close_selection_menu()
{
	if (auto sm = this->selection_menu.lock()) {
		this->context.get().post_to_ui_thread([sm = std::move(sm)]() {
			sm->remove_from_parent();
		});
	}
	this->selection_menu.reset();
}

utki::shared_ref<ruis::touch::selection_box> ruis::touch::make::selection_box(
	utki::shared_ref<context> context, //
	ruis::touch::selection_box::all_parameters params
)
{
	return utki::make_shared<ruis::touch::selection_box>(
		std::move(context), //
		std::move(params)
	);
}
