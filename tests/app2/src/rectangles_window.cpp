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

#include "rectangles_window.hpp"

#include <ruis/widget/button/impl/rectangle_push_button.hpp>
#include <ruis/widget/group/window.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/label/rectangle.hpp>
#include <ruis/widget/label/text.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m = ruis::make;

utki::shared_ref<ruis::widget> make_rectangles_window(
	const utki::shared_ref<ruis::context>& c, //
	ruis::vec2_length pos
)
{
	// clang-format off
	return m::window(c,
		{
			.widget_params = {
				.rectangle = {
					{
						pos.x().get(c),
						pos.y().get(c)
					},
					{
						ruis::length::make_pp(300).get(c),
						ruis::length::make_pp(200).get(c)
					}
				}
			},
			.container_params = {
				.layout = ruis::layout::column
			},
			.title = c.get().localization.get().get("rectangles"sv)
		},
		{
			m::rectangle(c,
				{
					.color_params{
						.color = 0xff800000
					}
				},
				{
					m::text(c, {}, U"no borders"s)
				}
			),
			m::gap(c,
				{
					.layout_params{
						.dims = {0_px, 5_pp}
					}
				}
			),
			m::rectangle(c,
				{
					.padding_params{
						.borders{
							5_pp, // left
							10_pp, // top
							15_pp, // right
							20_pp // bottom
						}
					},
					.color_params{
						.color = 0xff800000
					}
				},
				{
					m::text(c, {}, U"borders"s)
				}
			),
			m::gap(c,
				{
					.layout_params{
						.dims = {0_px, 5_pp}
					}
				}
			),
			m::rectangle(c,
				{
					.padding_params{
						.borders{
							5_pp, // left
							10_pp, // top
							15_pp, // right
							20_pp // bottom
						}
					},
					.color_params{
						.color = 0xff800000
					},
					.rectangle_params{
						.corner_radii = {
							0, // left-top
							5_pp, // right-top
							10_pp, // right-bottom
							0 // left-bottom
						}
					}
				},
				{
					m::text(c, {}, U"one side square corners"s)
				}
			),
			m::gap(c,
				{
					.layout_params{
						.dims = {0_px, 5_pp}
					}
				}
			),
			m::rectangle(c,
				{
					.padding_params{
						.borders{
							5_pp, // left
							10_pp, // top
							15_pp, // right
							20_pp // bottom
						}
					},
					.color_params{
						.color = 0xff800000
					},
					.rectangle_params{
						.corner_radii = {
							0, // left-top
							5_pp, // right-top
							10_pp, // right-bottom
							15_pp // left-bottom
						}
					}
				},
				{
					m::text(c, {}, U"rounded corners"s)
				}
			),
			m::gap(c,
				{
					.layout_params{
						.dims = {0_px, 5_pp}
					}
				}
			),
			m::rectangle_push_button::push_button(c,
				{
					.padding_params{
						.borders{
							5_pp, // left
							10_pp, // top
							15_pp, // right
							20_pp // bottom
						}
					},
					.rectangle_params{
						.corner_radii = {5_pp}
					},
					.rectangle_button_params{
						// TODO:
					}
				},
				{
					m::text(c, {}, U"rounded corners button"s)
				}
			)
		}
	);
	// clang-format on
}
