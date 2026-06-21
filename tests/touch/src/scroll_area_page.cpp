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

#include "scroll_area_page.hpp"

#include <ruis/res/tml.hpp>
#include <ruis/standard_widgets.hpp>
#include <ruis/style/style_sheet.hpp>
#include <ruis/widget/button/impl/rectangle_push_button.hpp>
#include <ruis/widget/group/touch/scroll_area.hpp>
#include <ruis/widget/label/text.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>

#include "style.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace {

void apply_theme(
	ruis::context& c, //
	ruis::theme theme
)
{
	auto style_res = c.loader().load<ruis::res::tml>(ruis::to_resource_id(theme));
	c.style().set(utki::make_shared<ruis::style_sheet>(style_res.get().forest));
}

class theme_selection_provider : public ruis::list_provider
{
	std::vector<std::u32string> items;

public:
	theme_selection_provider(utki::shared_ref<ruis::context> context) :
		ruis::list_provider(std::move(context)),
		items{U"Dark"s, U"Light"s}
	{}

	size_t count() const noexcept override
	{
		return this->items.size();
	}

	utki::shared_ref<ruis::widget> get_widget(size_t index) override
	{
		// clang-format off
		return m::text(this->context,
			{
				.color_params{
					.color = this->context.get().style().get_color_text_normal()
				}
			},
			this->items.at(index)
		);
		// clang-format on
	}

	utki::shared_ref<ruis::widget> get_highlighted_widget(size_t index) override
	{
		// clang-format off
		return m::text(this->context,
			{
				.color_params{
					.color = this->context.get().style().get_color_text_highlight()
				}
			},
			this->items.at(index)
		);
		// clang-format on
	}
};

ruis::widget_list make_scroll_area_page_contents(utki::shared_ref<ruis::context> c)
{
	// clang-format off
	auto theme_selector = m::selection_box(c,
		{
			.layout_params{
				.dims = {ruis::dim::max, ruis::dim::min}
			},
			.list_params{
				.provider = utki::make_shared<theme_selection_provider>(c)
			},
			.title = U"Theme"s // TODO: localize
		}
	);
	// clang-format on

	theme_selector.get().set_selection(size_t(ruis::theme::dark));
	theme_selector.get().selection_handler = [](ruis::selection_box& sb) {
		apply_theme(
			sb.context.get(), //
			ruis::theme(sb.get_selection())
		);
	};

	// clang-format off
	auto button_1 = m::push_button(c,
		{
			.layout_params{
				.dims = {ruis::dim::fill, 200_pp}
			}
		},
		{
			m::text(c, {}, U"Button 1"s)
		}
	);
	// clang-format on

	button_1.get().click_handler = [](ruis::push_button&) {
		utki::log([](auto& o) {
			o << "Button 1 clicked\n";
		});
	};

	// clang-format off
	return {
		theme_selector,
		button_1,
		m::scroll_bar(c,
			{
				.layout_params{
					.dims{50_pp, 1000_pp}
				},
				.fraction_params{
					.fraction = ruis::real(0.25)
				},
				.fraction_band_params{
					.band_fraction = ruis::real(0.2)
				},
				.oriented_params{
					.vertical = true
				}
			}
		),
		m::text(c, {}, U"some text"s)
	};
	// clang-format on
}

class scroll_area_page :
	public ruis::page, //
	private ruis::touch::scroll_area
{
public:
	scroll_area_page(utki::shared_ref<ruis::context> c) :
		// clang-format off
		ruis::widget(
			std::move(c),
			{},
			{
				.clip = true
			}
		),
		// clang-format on
		ruis::page(this->context, {}),
		// clang-format off
		ruis::touch::scroll_area(
			this->context,
			{},
			{
				m::column(
					this->context,
					{
						.layout_params{
							.dims = {ruis::dim::fill, ruis::dim::min}
						}
					},
					make_scroll_area_page_contents(this->context)
				)
			}
		)
	// clang-format on
	{}
};

} // namespace

utki::shared_ref<ruis::page> make_scroll_area_page(utki::shared_ref<ruis::context> c)
{
	return utki::make_shared<scroll_area_page>(std::move(c));
}
