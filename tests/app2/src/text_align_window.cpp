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

#include "text_align_window.hpp"

#include <array>

#include <ruis/widget/group/window.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/label/padding.hpp>
#include <ruis/widget/label/rectangle.hpp>
#include <ruis/widget/label/text.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m = ruis::make;

namespace {
// A visible box (a filled rectangle) containing a text label aligned within it.
// The label is given an explicit height (or dim::min for the natural height) so that
// the widget is bigger than the text and the alignment effect is visible.
//
// 'weight' is the box's layout weight: in a row layout the boxes need a non-zero
// weight to share the row's width between themselves (a 'fill' dim in the line
// direction alone gives the box a size of zero).
utki::shared_ref<ruis::widget> make_align_box(
	const utki::shared_ref<ruis::context>& c, //
	ruis::align h_align, //
	ruis::align v_align, //
	std::u32string_view text, //
	ruis::dimension label_height, //
	bool multiline = false, //
	bool marquee = false, //
	ruis::real weight = 0
)
{
	// clang-format off
	return m::rectangle(c,
		{
			.layout_params{
				.dims = {ruis::dim::fill, ruis::dim::min},
				.weight = weight
			},
			.params{
				.padding{
					.specific{
						.borders = {4_pp, 4_pp, 4_pp, 4_pp}
					}
				},
				.specific{
					.fill_color = 0xff282830
				}
			}
		},
		{
			m::text(c,
				{
					.layout_params{
						.dims = {ruis::dim::fill, label_height},
						// aligns the label within the box when the box is clamped to a size
						// smaller than the label (e.g. when the window is shrunk): the label
						// sticks to the same edge the text is aligned to, instead of being
						// centered in the box
						.align = {h_align, v_align}
					},
					.widget{
						.clip = true
					},
					.params{
						.specific{
							.multiline = multiline,
							.marquee = marquee,
							.align = {h_align, v_align}
						}
					}
				},
				std::u32string(text)
			)
		}
	);
	// clang-format on
}
} // namespace

namespace {
constexpr std::array<ruis::align, 3> aligns = {
	ruis::align::front, //
	ruis::align::center, //
	ruis::align::back //
};
} // namespace

utki::shared_ref<ruis::widget> make_text_align_window(
	const utki::shared_ref<ruis::context>& c, //
	ruis::vec2_length pos
)
{
	// A grid row of three boxes: the horizontal alignment varies per column (front,
	// center, back), the vertical alignment is the same for the whole row.
	const auto make_grid_row = [&](ruis::align v_align) {
		ruis::widget_list children;
		for (const auto h_align : aligns) {
			// weight 1: each box gets an equal share of the row's width
			children.push_back(make_align_box(c, h_align, v_align, U"align", 45_pp, false, false, 1));
		}
		return m::row(c, {.layout_params{.dims = {ruis::dim::fill, ruis::dim::min}}}, children);
	};

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
						ruis::length::make_pp(540).get(c)
					}
				}
			},
			.params = {
				.layout = ruis::layout::column
			},
			.title = U"text align"
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
							.borders = {8_pp, 8_pp, 8_pp, 8_pp}
						}
					}
				},
				{
					// a 3x3 grid of single line labels:
					// columns = horizontal alignment (front, center, back),
					// rows = vertical alignment (front, center, back)
					make_grid_row(ruis::align::front),
					m::gap(c,
						{
							.layout_params{
								.dims = {0_px, 5_pp}
							}
						}
					),
					make_grid_row(ruis::align::center),
					m::gap(c,
						{
							.layout_params{
								.dims = {0_px, 5_pp}
							}
						}
					),
					make_grid_row(ruis::align::back),
					m::gap(c,
						{
							.layout_params{
								.dims = {0_px, 8_pp}
							}
						}
					),

					// single line with the back (right) horizontal alignment
					make_align_box(
						c, //
						ruis::align::back, //
						ruis::align::front, //
						U"Single line right aligned.", //
						40_pp
					),
					m::gap(c,
						{
							.layout_params{
								.dims = {0_px, 8_pp}
							}
						}
					),

					// multiline with the back (right) horizontal alignment:
					// every line is aligned to the right edge of the box
					make_align_box(
						c, //
						ruis::align::back, //
						ruis::align::front, //
						U"Multiline right aligned: every line is aligned to the right edge of the box.", //
						50_pp, //
						true
					),
					m::gap(c,
						{
							.layout_params{
								.dims = {0_px, 8_pp}
							}
						}
					),

					// single line with the back (bottom) vertical alignment:
					// the line is aligned to the bottom edge of the box
					make_align_box(
						c, //
						ruis::align::front, //
						ruis::align::back, //
						U"Single line bottom aligned.", //
						55_pp
					),
					m::gap(c,
						{
							.layout_params{
								.dims = {0_px, 8_pp}
							}
						}
					),

					// multiline with the back (bottom) vertical alignment in a box that is
					// shorter than the text: the last lines are aligned to the box's bottom
					// edge and the first lines are clipped by the box's top edge
					make_align_box(
						c, //
						ruis::align::front, //
						ruis::align::back, //
						U"Multiline bottom aligned: this paragraph is taller than its box. The quick brown fox jumps over the lazy dog. Pack my box with five dozen liquor jugs. The last lines are visible, the first lines are clipped by the box top edge.", //
						55_pp, //
						true
					),
					m::gap(c,
						{
							.layout_params{
								.dims = {0_px, 8_pp}
							}
						}
					),

					// single line marquee with the back (right) horizontal alignment:
					// the marquee always starts from the beginning of the line,
					// the alignment has no effect on the scrolling line
					make_align_box(
						c, //
						ruis::align::back, //
						ruis::align::front, //
						U"Single line marquee with back (right) alignment: the marquee always starts from the beginning of the line.", //
						ruis::dim::min, //
						false, //
						true
					),
					m::gap(c,
						{
							.layout_params{
								.dims = {0_px, 8_pp}
							}
						}
					),

					// multiline marquee with the back (right) horizontal alignment:
					// every line wider than the box scrolls from its beginning,
					// the alignment has no effect on the scrolling lines
					make_align_box(
						c, //
						ruis::align::back, //
						ruis::align::front, //
						U"Multiline marquee with back (right) alignment: Supercalifragilisticexpialidocious is wider than the box and so is this very long line of text. The lines that fit are right aligned.", //
						ruis::dim::min, //
						true, //
						true
					)
				}
			)
		}
	);
	// clang-format on
}
