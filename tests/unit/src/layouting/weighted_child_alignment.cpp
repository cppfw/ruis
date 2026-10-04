#include <ruis/context.hpp>
#include <ruis/util/length.hpp>
#include <ruis/widget/container.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/proxy/mouse_proxy.hpp>
#include <tst/check.hpp>
#include <tst/set.hpp>

#include "../../../harness/util/dummy_context.hpp"

using namespace ruis;

namespace {
// A widget with a given natural size: a pile containing a fixed-size proxy.
utki::shared_ref<ruis::widget> natural_size(
	const utki::shared_ref<ruis::context>& c, //
	real w, //
	real h
)
{
	return ruis::make::pile(
		c,
		{
			.layout_params{
				.dims = {ruis::dim::min, ruis::dim::min}
			}
		},
		{
			ruis::make::mouse_proxy(
				c,
				{
					.layout_params{
						.dims = {
							ruis::dim(ruis::length::make_pp(w)), //
							ruis::dim(ruis::length::make_pp(h))
						}
					}
				}
			)
		}
	);
}

// A container with pile layout and the given layout parameters containing a
// single natural-size widget.
utki::shared_ref<ruis::widget> field(
	const utki::shared_ref<ruis::context>& c, //
	layout_parameters lp, //
	real w, //
	real h
)
{
	return ruis::make::container(
		c,
		{
			.layout_params = std::move(lp),
			.params{
				.layout = ruis::layout::pile
			}
		},
		{
			natural_size(c, w, h)
		}
	);
}
} // namespace

namespace {
const tst::set set("weighted_child_alignment", [](tst::suite& suite) {
	// Regression test: a weighted child which is 'min' in the longitudinal direction
	// gets its natural size, but it still allocates a room (natural size + its share of
	// the flexible space) which is used for aligning it (front/center/back) and for
	// advancing the position of the following children.
	suite.add("weighted_back_aligned_child_is_placed_at_the_back_of_its_room__column", [] {
		auto context = make_dummy_context();

		// The structure of the calslog dialogs: a fixed-height column of fields with a
		// weighted button row at the end which is back aligned in the longitudinal
		// (vertical) direction, so that the buttons are at the bottom of the column.
		// clang-format off
		auto column = ruis::make::column(context,
			{
				.layout_params{
					.dims = {ruis::dim::fill, ruis::dim::fill}
				}
			},
			{
				field(context,
					{
						.dims = {ruis::dim::fill, ruis::dim::min}
					},
					100,
					30),
				ruis::make::gap(context,
					{
						.layout_params{
							.dims = {ruis::dim::fill, ruis::dim(ruis::length::make_pp(8))}
						}
					}),
				field(context,
					{
						.dims = {ruis::dim::fill, ruis::dim::min}
					},
					100,
					40),
				ruis::make::gap(context,
					{
						.layout_params{
							.dims = {ruis::dim::fill, ruis::dim(ruis::length::make_pp(8))}
						}
					}),
				field(context,
					{
						.dims = {ruis::dim::fill, ruis::dim::min}
					},
					100,
					40),
				ruis::make::gap(context,
					{
						.layout_params{
							.dims = {ruis::dim::fill, ruis::dim(ruis::length::make_pp(8))}
						}
					}),
				field(context,
					{
						.dims = {ruis::dim::fill, ruis::dim::min}
					},
					100,
					20),
				ruis::make::row(context,
					{
						.layout_params{
							.dims = {ruis::dim::fill, ruis::dim::min},
							.weight = 1,
							.align = {ruis::align::front, ruis::align::back}
						}
					},
					{
						field(context,
							{
								.dims = {ruis::dim::fill, ruis::dim::min},
								.weight = 1
							},
							60,
							30),
						ruis::make::gap(context,
							{
								.layout_params{
									.dims = {ruis::dim(ruis::length::make_pp(8)), ruis::dim::fill}
								}
							}),
						field(context,
							{
								.dims = {ruis::dim::fill, ruis::dim::min},
								.weight = 1
							},
							60,
							30)
					})
			}
		);
		// clang-format on

		auto& col = column.get();
		col.resize(ruis::vec2(300, 500));
		col.lay_out();

		// The natural height of the fields and gaps: 30+8+40+8+40+8+20 = 154.
		// The row has weight 1 and takes all the remaining space as its room, but its
		// own height is its natural height (30). Being back aligned in the vertical
		// direction it is placed at the bottom of the column: y = 500 - 30 = 470.
		auto& row = col.children().back().get();
		tst::check_eq(row.rect().d[0], real(300), SL);
		tst::check_eq(row.rect().d[1], real(30), SL);
		tst::check_eq(row.rect().p[1], real(470), SL);
	});

	suite.add("weighted_back_aligned_child_is_placed_at_the_back_of_its_room__row", [] {
		auto context = make_dummy_context();

		// The structure of the touch selection box: a row with a title on the left
		// (front aligned) and a weighted container holding the selected value which
		// is back aligned, so that the value is at the right edge of the row.
		// clang-format off
		auto row = ruis::make::row(context,
			{
				.layout_params{
					.dims = {ruis::dim::fill, ruis::dim::fill}
				}
			},
			{
				field(context,
					{
						.dims = {ruis::dim::min, ruis::dim::min},
						.align = {ruis::align::front, ruis::align::center}
					},
					40,
					20),
				ruis::make::pile(context,
					{
						.layout_params{
							.dims = {ruis::dim::min, ruis::dim::min},
							.weight = 1,
							.align = {ruis::align::back, ruis::align::center}
						}
					},
					{
						natural_size(context, 50, 20)
					})
			}
		);
		// clang-format on

		auto& r = row.get();
		r.resize(ruis::vec2(200, 40));
		r.lay_out();

		// The title is at the left edge: x = 0.
		auto& title = r.children().front().get();
		tst::check_eq(title.rect().p[0], real(0), SL);
		tst::check_eq(title.rect().d[0], real(40), SL);

		// The value takes its natural width (50) but its room is the remaining space
		// (160). Being back aligned in the horizontal direction it is placed at the
		// right edge: x = 200 - 50 = 150.
		auto& value = r.children().back().get();
		tst::check_eq(value.rect().d[0], real(50), SL);
		tst::check_eq(value.rect().p[0], real(150), SL);

		// Both children are center aligned in the vertical direction: y = (40 - 20) / 2 = 10.
		tst::check_eq(title.rect().p[1], real(10), SL);
		tst::check_eq(value.rect().p[1], real(10), SL);
	});
});
} // namespace
