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
						.color = 0x80000000 // TODO: use color from theme
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

						}
					),
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
					)
				}
			)
		}
	);
	// clang-format on

	bg_click_proxy.get().click_handler = [weak_root = utki::make_weak(root)](ruis::click_proxy& cp) {
		std::cout << "clicked" << std::endl;
		if (auto r = weak_root.lock()) {
			r->context.get().post_to_ui_thread([r]() {
				r->remove_from_parent();
			});
		}
	};

	c.get().post_to_ui_thread([olay = utki::make_shared_from(olay), root]() {
		olay.get().push_back(root);
	});
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
