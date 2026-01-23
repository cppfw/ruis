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

#include "gui.hpp"

#include <ruis/res/image.hpp>
#include <ruis/widget/group/overlay.hpp>
#include <ruis/widget/group/touch/tabbed_book.hpp>
#include <ruis/widget/label/image.hpp>
#include <ruis/widget/label/padding.hpp>
#include <ruis/widget/label/text.hpp>

#include "list_page.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m {

using namespace ruis::make;

using ruis::touch::make::tabbed_book;

}; // namespace m

namespace {

class tab_button :
	public ruis::choice_button, //
	private ruis::container
{
public:
	struct all_parameters {
		ruis::layout::parameters layout_params;
		ruis::widget::parameters widget_params;
		ruis::image::parameters image_params;
		ruis::string text;
	};

	tab_button(
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
		toggle_button(this->context),
		choice_button(this->context),
		// clang-format off
		ruis::container(
			this->context, //
			{
				.container_params = {
					.layout = ruis::layout::pile
				}
			},
			{
				m::padding(
					this->context,
					{
						.layout_params = {
							.dims = {ruis::dim::fill, ruis::dim::fill}
						},
						.container_params = {
							.layout = ruis::layout::column
						},
						.padding_params = {
							.borders = {10_pp}
						}
					},
					{
						m::image(this->context,
							{
								.layout_params = {
									.dims = {ruis::dim::min, ruis::dim::fill},
									.weight = 1
								},
								.image_params = std::move(params.image_params)
							}
						),
						m::text(this->context,
							{
								.layout_params = {
									.dims = {ruis::dim::min, ruis::dim::min},
									.align = {ruis::align::center, ruis::align::center}
								}
							},
							std::move(params.text)
						)
					}
				)
			}
		)
	// clang-format on
	{}

	bool on_mouse_button(const ruis::mouse_button_event& event) override
	{
		return this->choice_button::on_mouse_button(event);
	}
};

utki::shared_ref<ruis::choice_button> make_tab_button(
	utki::shared_ref<ruis::context> c, //
	utki::shared_ref<const ruis::res::image> icon,
	ruis::string text
)
{
	// clang-format off
	return utki::make_shared<tab_button>(std::move(c),
		tab_button::all_parameters{
			.layout_params = {
				.dims = {ruis::dim::fill, 60_pp},
				.weight = 1
			},
			.image_params = {
				.img = std::move(icon),
				.keep_aspect_ratio = true
			},
			.text = std::move(text)
		}
	);
	// clang-format on
}

} // namespace

utki::shared_ref<ruis::widget> make_root_widget(utki::shared_ref<ruis::context> c)
{
	// clang-format off
	return m::overlay(c,
		{
			.layout_params = {
				.dims = {ruis::dim::fill, ruis::dim::fill}
			}
		},
		{
			m::tabbed_book(c,
				{
					.layout_params = {
						.dims = {ruis::dim::fill, ruis::dim::fill}
					}
				},
				{
					{
						make_tab_button(c, c.get().loader().load<ruis::res::image>("img_home"), U"Home"s),
						make_list_page(c)
					},
					{
						make_tab_button(c, c.get().loader().load<ruis::res::image>("img_home"), U"List"s),
						make_list_page(c)
					},
					{
						make_tab_button(c, c.get().loader().load<ruis::res::image>("img_home"), U"Stuff"s),
						make_list_page(c)
					}
				}
			)
		}
	);
	// clang-format on
}
