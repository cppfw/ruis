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

#include "rectangle_radii_window.hpp"

#include <ruis/widget/group/window.hpp>
#include <ruis/widget/label/rectangle.hpp>
#include <ruis/widget/label/text.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m = ruis::make;

// A window which content is a stroked rectangle wrapping a filled rectangle,
// both with per-corner radii which collide with each other when the window is
// small enough.
//
// The filled rectangle's left-top and right-top radii are identical, the
// right-bottom and left-bottom are different (and all four are distinct from
// each other except for the top pair). The radii are large enough to collide at
// the initial window size, so the normalized (scaled-down) result is visible
// immediately, and resizing the window with the mouse (drag a border or a corner)
// shows how the corner radii are re-distributed as the available edge lengths
// change.
//
// The stroked rectangle wraps the filled one and is inset from its edge by a
// padding (its 'borders'), so the stroke does not cover the filled rectangle's
// edge. Its corner radii are the filled radii plus that padding, which keeps the
// two sets of arcs concentric.
utki::shared_ref<ruis::widget> make_rectangle_radii_window(
	const utki::shared_ref<ruis::context>& c, //
	ruis::vec2_length pos
)
{
	// padding between the filled rect and the stroke (insets the filled rect)
	constexpr auto gap = 10_pp;

	// clang-format off
	return m::window(c,
		{
			.widget = {
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
			.params = {
				.layout = ruis::layout::pile
			},
			.title = c.get().localization.get().get("rectangle_radii"sv)
		},
		{
			m::rectangle(c,
				{
					.layout_params{
						.dims = {ruis::dim::fill, ruis::dim::fill}
					},
					.params{
						.padding{
							.specific{
								// inset the inner filled rect so the stroke does not
								// cover its edge
								.borders = {gap}
							}
						},
						.specific{
							// concentric with the filled rect: filled radii + gap
							.corner_radii = {
								110_pp, // left-top     (100 + 10)
								110_pp, // right-top    (100 + 10)
								160_pp, // right-bottom (150 + 10)
								60_pp  // left-bottom  (50 + 10)
							},
							.fill_color = 0x00000000, // transparent, only the stroke shows
							.stroke_width = 4_pp,
							.stroke_color = 0xff33ccff
						}
					}
				},
				{
					m::rectangle(c,
						{
							.layout_params{
								.dims = {ruis::dim::fill, ruis::dim::fill}
							},
							.params{
								.specific{
									.corner_radii = {
										100_pp, // left-top    (identical to right-top)
										100_pp, // right-top   (identical to left-top)
										150_pp, // right-bottom (different)
										50_pp  // left-bottom  (different)
									},
									.fill_color = 0xff0060a0
								}
							}
						},
						{
							m::text(c,
								{
									.layout_params{
										.align = {ruis::align::center, ruis::align::center}
									}
								},
								U"radii: lt=100 rt=100 rb=150 lb=50"s
							)
						}
					)
				}
			)
		}
	);
	// clang-format on
}
