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
		{.layout_params{.dims = {ruis::dim::min, ruis::dim::min}}},
		{ruis::make::mouse_proxy(
			c,
			{.layout_params{
				.dims =
					{ruis::dim(ruis::length::make_pp(w)), //
					 ruis::dim(ruis::length::make_pp(h))}
			}}
		)}
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
		{.layout_params = std::move(lp), .params{.layout = ruis::layout::pile}},
		{natural_size(c, w, h)}
	);
}
} // namespace

namespace {
// NOLINTNEXTLINE(cppcoreguidelines-interfaces-global-init, "false-positive: the set depends on global ruis::dim::fill which might be uninitialized at the time of static initialization of this set, but it is initialized at the time of tests execution")
const tst::set set("linear_layout__weighted_child", [](tst::suite& suite) {
	// A child is always resized to the space assigned to it in the longitudinal
	// direction: its room (natural size + its share of the flexible space) clamped to
	// the space left. Because it fills the whole room, the longitudinal alignment has
	// no effect, only the transverse alignment does.

	suite.add("weighted_child_fills_its_room__column", [] {
		auto context = make_dummy_context();

		// The structure of the calslog dialogs: a fixed-height column of fields with a
		// weighted button row at the end. The row is back aligned in the longitudinal
		// (vertical) direction, which makes no difference: the row fills the whole
		// space left over by the fields.
		// clang-format off
		auto row = ruis::make::row(context,
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
			}
		);

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
				row
			}
		);
		// clang-format on

		auto& col = column.get();
		col.resize(ruis::vec2(300, 500));
		col.lay_out();

		// The natural height of the fields and gaps: 30+8+40+8+40+8+20 = 154.
		// The row has weight 1, so its room is all the remaining space (500 - 154 = 346)
		// and it is resized to it, reaching the bottom of the column.
		auto& r = row.get();
		tst::check_eq(r.rect().d[0], real(300), SL);
		tst::check_eq(r.rect().d[1], real(346), SL);
		tst::check_eq(r.rect().p[1], real(154), SL);

		// Inside the row the two weighted fields split its width (300 - 8 = 292) and keep
		// their natural height, center aligned in the transverse (vertical) direction
		// (undefined alignment defaults to center): y = (346 - 30) / 2 = 158.
		auto& first_button = r.children().front().get();
		tst::check_eq(first_button.rect().d[0], real(146), SL);
		tst::check_eq(first_button.rect().d[1], real(30), SL);
		tst::check_eq(first_button.rect().p[1], real(158), SL);
	});

	suite.add("weighted_child_fills_its_room__row", [] {
		auto context = make_dummy_context();

		// A row with a title on the left and a weighted container holding the selected
		// value, back aligned in the longitudinal (horizontal) direction. The alignment
		// makes no difference: the value fills the whole space left over by the title.
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

		// The unweighted title keeps its natural width and is at the left edge: x = 0.
		auto& title = r.children().front().get();
		tst::check_eq(title.rect().p[0], real(0), SL);
		tst::check_eq(title.rect().d[0], real(40), SL);

		// The value has weight 1, so its room is the remaining space (200 - 40 = 160)
		// and it is resized to it: x = 40, width = 160.
		auto& value = r.children().back().get();
		tst::check_eq(value.rect().d[0], real(160), SL);
		tst::check_eq(value.rect().p[0], real(40), SL);

		// Both children are center aligned in the transverse (vertical) direction: y = (40 - 20) / 2 = 10.
		tst::check_eq(title.rect().p[1], real(10), SL);
		tst::check_eq(value.rect().p[1], real(10), SL);
	});
});
} // namespace
