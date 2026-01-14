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
#include <ruis/widget/button/touch/tab_group.hpp>
#include <ruis/widget/group/overlay.hpp>
#include <ruis/widget/label/image.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m {

using namespace ruis::make;

using ruis::touch::make::tab_group;

}; // namespace m

namespace {

class tab_button :
	public ruis::choice_button, //
	public ruis::image
{
public:
	tab_button(
		utki::shared_ref<ruis::context> context, //
		ruis::image::all_parameters params
	) :
		widget(
			std::move(context), //
			std::move(params.layout_params),
			std::move(params.widget_params)
		),
		button(this->context, {}),
		toggle_button(this->context),
		choice_button(this->context),
		ruis::image(
			this->context, //
			std::move(params)
		)
	{}
};

utki::shared_ref<ruis::widget> make_tab_button(
	utki::shared_ref<ruis::context> c, //
	utki::shared_ref<const ruis::res::image> icon
)
{
	// clang-format off
	return utki::make_shared<tab_button>(std::move(c),
		ruis::image::all_parameters{
			.layout_params = {
				.dims = {ruis::dim::fill, ruis::dim::fill},
				.weight = 1
			},
			.image_params = {
				.img = std::move(icon)
			}
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
			m::tab_group(c,
				{
					.layout_params = {
						.dims = {ruis::dim::fill, 60_pp}
					}
				},
				{
					make_tab_button(c, c.get().loader().load<ruis::res::image>("img_home")),
					make_tab_button(c, c.get().loader().load<ruis::res::image>("img_home")),
					make_tab_button(c, c.get().loader().load<ruis::res::image>("img_home")),
				}
			)
		}
	);
	// clang-format on
}
