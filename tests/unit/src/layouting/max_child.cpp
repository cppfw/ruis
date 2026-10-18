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
const tst::set set("linear_layout__max_child", [](tst::suite& suite) {
	// A 'max' dimension is measured as 'min', but is laid out as 'fill': a 'max' child
	// does not keep its natural size as a baseline, it gets its share of the space left
	// over by its siblings. Like a 'fill' child, it participates in the longitudinal
	// distribution only through its weight.

	suite.add("weighted_max_child_shrinks_under_deficit__column", [] {
		auto context = make_dummy_context();

		// The structure of the calslog day entries dialog: a weighted list which is
		// taller than the dialog, followed by a button row which must stay fully visible.
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
						.dims = {ruis::dim::fill, ruis::dim::max},
						.weight = 1
					},
					100,
					400),
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
					50)
			}
		);
		// clang-format on

		auto& col = column.get();
		col.resize(ruis::vec2(300, 300));
		col.lay_out();

		// The weighted 'max' child has no rigid size, so the space left for it is
		// 300 - 8 - 50 = 242, which is less than its natural size (400): it shrinks.
		auto& list = col.children().front().get();
		tst::check_eq(list.rect().d[1], real(242), SL);
		tst::check_eq(list.rect().p[1], real(0), SL);

		// The button row keeps its natural size and is not squeezed out.
		auto& button = col.children().back().get();
		tst::check_eq(button.rect().d[1], real(50), SL);
		tst::check_eq(button.rect().p[1], real(250), SL);
	});

	suite.add("weighted_max_child_grows_into_spare_space__column", [] {
		auto context = make_dummy_context();

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
						.dims = {ruis::dim::fill, ruis::dim::max},
						.weight = 1
					},
					100,
					100),
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
					50)
			}
		);
		// clang-format on

		auto& col = column.get();
		col.resize(ruis::vec2(300, 300));
		col.lay_out();

		// Being laid out as 'fill', the weighted 'max' child takes all the space left
		// over by its siblings (300 - 8 - 50 = 242), growing beyond its natural size (100).
		auto& child = col.children().front().get();
		tst::check_eq(child.rect().d[1], real(242), SL);
	});

	suite.add("unweighted_max_child_gets_zero_long_size__column", [] {
		auto context = make_dummy_context();

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
						.dims = {ruis::dim::fill, ruis::dim::max}
					},
					100,
					400),
				field(context,
					{
						.dims = {ruis::dim::fill, ruis::dim::min}
					},
					100,
					50)
			}
		);
		// clang-format on

		auto& col = column.get();
		col.resize(ruis::vec2(300, 300));
		col.lay_out();

		// An unweighted 'max' child is laid out as an unweighted 'fill' child: it gets no
		// share of the flexible space, so its longitudinal size is 0.
		auto& child = col.children().front().get();
		tst::check_eq(child.rect().d[1], real(0), SL);

		auto& next = col.children().back().get();
		tst::check_eq(next.rect().d[1], real(50), SL);
	});

	suite.add("max_child_is_measured_as_min", [] {
		auto context = make_dummy_context();

		// clang-format off
		auto column = ruis::make::column(context,
			{
				.layout_params{
					.dims = {ruis::dim::fill, ruis::dim::min}
				}
			},
			{
				field(context,
					{
						.dims = {ruis::dim::fill, ruis::dim::max},
						.weight = 1
					},
					100,
					400),
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
					50)
			}
		);
		// clang-format on

		// Measuring the container as 'min' reports the natural sizes of the children,
		// including the weighted 'max' child: 400 + 8 + 50 = 458.
		auto size = column.get().measure(
			{measure_infinite_quotum, measure_infinite_quotum},
			{measure_mode::at_most, measure_mode::at_most}
		);

		tst::check_eq(size[1], real(458), SL);
	});
});
} // namespace
