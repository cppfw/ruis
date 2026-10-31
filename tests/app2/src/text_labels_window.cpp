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

#include "text_labels_window.hpp"

#include <ruis/widget/group/window.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/label/padding.hpp>
#include <ruis/widget/label/text.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m = ruis::make;

utki::shared_ref<ruis::widget> make_text_labels_window(
	const utki::shared_ref<ruis::context>& c, //
	ruis::vec2_length pos
)
{
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
						ruis::length::make_pp(320).get(c),
						ruis::length::make_pp(420).get(c)
					}
				}
			},
			.params = {
				.layout = ruis::layout::column
			},
			.title = U"text labels"
		},
		{
			m::padding(c,
				{
					.layout_params{
						.dims = {ruis::dim::fill, ruis::dim::min}
					},
					.params{
						.container{
							.layout = ruis::layout::column
						},
						.specific{
							.borders = {10_pp, 10_pp, 10_pp, 10_pp}
						}
					}
				},
				{
					// a short text that fits on a single line and is not wrapped
					m::text(c,
						{
							.layout_params{
								.dims = {ruis::dim::fill, ruis::dim::min}
							}
						},
						U"Short text, fits on one line."
					),
					m::gap(c,
						{
							.layout_params{
								.dims = {0_px, 5_pp}
							}
						}
					),

					// a long single line truncated with an ellipsis (the default behaviour)
					m::text(c,
						{
							.layout_params{
								.dims = {ruis::dim::fill, ruis::dim::min}
							}
						},
						U"Single line, too long to fit, so it is truncated with an ellipsis."
					),
					m::gap(c,
						{
							.layout_params{
								.dims = {0_px, 5_pp}
							}
						}
					),

					// a long single line that scrolls back and forth (marquee)
					m::text(c,
						{.layout_params{.dims = {ruis::dim::fill, ruis::dim::min}}, .params{.specific{.marquee = true}}},
						U"Single line marquee that scrolls back and forth across the window."
					),
					m::gap(c,
						{
							.layout_params{
								.dims = {0_px, 5_pp}
							}
						}
					),

					// a long paragraph that wraps to multiple lines (words are not broken)
					m::text(c,
						{.layout_params{.dims = {ruis::dim::fill, ruis::dim::min}}, .params{.specific{.multiline = true}}},
						U"Multiline text wraps to several lines. The quick brown fox jumps over the lazy dog. Pack my box with five dozen liquor jugs."
					),
					m::gap(c,
						{
							.layout_params{
								.dims = {0_px, 5_pp}
							}
						}
					),

					// a paragraph that wraps and additionally scrolls the overflowing lines (marquee)
					m::text(c,
						{.layout_params{.dims = {ruis::dim::fill, ruis::dim::min}}, .params{.specific{.multiline = true,
								.marquee = true}}},
						U"Multiline marquee wraps to several lines. Supercalifragilisticexpialidocious is too wide for one line so it is truncated, while the overflowing lines scroll back and forth."
					)
				}
			)
		}
	);
	// clang-format on
}
