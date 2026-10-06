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

#include "list_page.hpp"

#include <ruis/util/widget_list.hpp>
#include <ruis/widget/button/impl/ellipse_push_button.hpp>
#include <ruis/widget/group/touch/list.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/label/image.hpp>
#include <ruis/widget/label/padding.hpp>
#include <ruis/widget/label/text.hpp>
#include <utki/string.hpp>
#include <utki/unicode.hpp>
#include <utki/unique_ref.hpp>

#include "context_menu.hpp"
#include "style.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace {
// A ruis::list_provider which provides the widgets of the context menu items.
class menu_provider : public ruis::list_provider
{
	ruis::widget_list items;

public:
	menu_provider(
		const utki::shared_ref<ruis::context>& context, //
		ruis::widget_list widgets
	) :
		ruis::list_provider(context) //
	{
		this->items = std::move(widgets);
	}

	size_t count() const noexcept override
	{
		return this->items.size();
	}

	utki::shared_ref<ruis::widget> get_widget(size_t index) const override
	{
		return this->items[index];
	}
};

class list_page_provider : public ruis::list_provider
{
public:
	list_page_provider(const utki::shared_ref<ruis::context>& context) :
		ruis::list_provider(context)
	{}

	size_t count() const noexcept override
	{
		return 100;
	}

	// Creates a context menu item widget: a text label with some padding around it.
	utki::shared_ref<ruis::widget> make_menu_item(ruis::string text) const
	{
		// clang-format off
		return m::padding(this->context,
			{
				.layout_params{
					.dims = {ruis::dim::min, ruis::dim::min}
				},
				.params{
					.container{
						.layout = ruis::layout::pile
					},
					.specific{
						.borders = {
							ruis::length::make_pp(12), // left
							ruis::length::make_pp(6), // top
							ruis::length::make_pp(12), // right
							ruis::length::make_pp(6) // bottom
						}
					}
				}
			},
			{
				m::text(this->context, {}, std::move(text))
			}
		);
		// clang-format on
	}

	utki::shared_ref<ruis::widget> get_widget(size_t index) const override
	{
		// clang-format off
		auto button = m::push_button(this->context,
			{
				.layout_params{
					.dims = {ruis::dim::min, ruis::length::make_pp(30)}
				}
			},
			{
				m::text(this->context, {}, U"Button"s)
			}
		);
		// clang-format on

		button.get().click_handler = [index](auto& btn) {
			std::cout << "Item #" << index << " button cliecked" << std::endl;
		};

		// Three dots button on the right side of the item; opens a context menu.
		// clang-format off
		auto menu_button = m::ellipse_push_button(this->context,
			{
				.layout_params{
					.dims = {ruis::dim::min, ruis::dim::fill},
					.align = {ruis::align::back, ruis::align::center}
				},
				.params{
					.ellipse_button{
						.ellipse{
							.padding{
								.specific{
									.borders = {ruis::length::make_pp(3)}
								}
							}
						},
						.specific{
							.unpressed_color = ruis::color::transparent
						}
					}
				}
			},
			{
				m::image(this->context,
					{
						.layout_params{
							.dims = {ruis::dim::min, ruis::dim::fill}
						},
						.params{
							.color = this->context.get().style().get_color_text(),
							.specific{
								.source = this->context.get().loader().load<ruis::res::image>("ruis_img_more"sv),
								.keep_aspect_ratio = true
							}
						}
					}
				)
			}
		);
		// clang-format on

		// When the three dots button is clicked, show a context menu near it.
		menu_button.get().click_handler = [this](auto& btn) {
			ruis::widget_list items = {
				this->make_menu_item(U"Edit"s), //
				this->make_menu_item(U"Copy"s), //
				this->make_menu_item(U"Share"s), //
				this->make_menu_item(U"Move to trash"s) //
			};
			context_menu::show(btn, utki::make_unique<menu_provider>(this->context, std::move(items)));
		};

		// clang-format off
		return m::column(this->context,
			{
				.layout_params{
					.dims = {ruis::dim::fill, ruis::dim::min}
				}
			},
			{
				m::padding(this->context,
					{
						.layout_params{
							.dims = {ruis::dim::fill, ruis::dim::min}
						},
						.params{
							.container{
								.layout = ruis::layout::row
							},
							.specific{
								.borders = {ruis::length::make_pp(3)}
							}
						}
					},
					{
						// Item content, fills the remaining width
						m::row(this->context,
							{
								.layout_params{
									.dims = {ruis::dim::fill, ruis::dim::min},
									.weight = 1
								}
							},
							{
								m::text(this->context, {}, utki::to_utf32(utki::cat("Item #", index))),
								m::gap(this->context,
									{
										.layout_params{
											.dims = {ruis::length::make_pp(5), ruis::dim::min}
										}
									}
								),
								std::move(button)
							}
						),
						// Gap before the three dots button
						m::gap(this->context,
							{
								.layout_params{
									.dims = {ruis::length::make_pp(3), ruis::dim::min}
								}
							}
						),
						// Three dots button on the right, vertically centered
						std::move(menu_button)
					}
				),
				m::gap(this->context,
					{
						.layout_params{
							.dims = {ruis::dim::fill, ruis::length::make_pp(1)}
						},
						.color = this->context.get().style().get_color_secondary()
					}
				)
			}
		);
		// clang-format on
	}
};
} // namespace

namespace {
class list_page :
	public ruis::page, //
	private ruis::touch::list
{
public:
	list_page(const utki::shared_ref<ruis::context>& context) :
		// clang-format off
		ruis::widget(context,
			{},
			{
				.clip = true
			}
		),
		// clang-format on
		ruis::page(context, {}),
		// clang-format off
		ruis::touch::list(context,
			{
				.params{
					.specific{
						.provider = utki::make_unique<list_page_provider>(context)
					}
				}
			}
		)
	// clang-format on
	{}
};

} // namespace

utki::shared_ref<ruis::page> make_list_page(const utki::shared_ref<ruis::context>& c)
{
	return utki::make_shared<list_page>(c);
}
