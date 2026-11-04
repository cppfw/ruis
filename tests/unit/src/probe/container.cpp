#include <ruis/widget/container.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/widget.hpp>
#include <tml/crawler.hpp>
#include <tml/tree.hpp>
#include <tst/check.hpp>
#include <tst/set.hpp>

#include "../../../harness/util/dummy_context.hpp"

namespace m {
using namespace ruis::make;
} // namespace m

namespace {
const tst::set set("container_probe", [](tst::suite& suite) {
	// Note: the probe response structure is deterministic, so the tests
	// navigate it with tml::const_crawler cascades - a structural mismatch
	// (regression) fails the test with an exception thrown by the crawler.
	//
	// The tests use ruis::container directly: it does not add a section of
	// its own to the 'get_state' fold, so the 'get_state' response is
	// exactly the base 'ruis::widget' section.

	suite.add("get_children", [] {
		auto ctx = make_dummy_context();

		auto child1 = m::gap(ctx, ruis::gap::all_parameters{});
		auto child2 = m::gap(ctx, ruis::gap::all_parameters{});
		auto child3 = m::gap(ctx, ruis::gap::all_parameters{});
		auto c = m::container(
			ctx,
			ruis::container::all_parameters{},
			ruis::widget_list{child1, child2, child3}
		);

		const auto resp = c.get().probe(tml::tree(tml::leaf("get_children")));

		const tml::forest f(resp.begin(), resp.end());

		// a non-fold command: a single section named after the container class
		tst::check(f.size() == 1, SL) << f.size();
		tst::check(f[0].value == "ruis::container", SL) << f[0].value.string;

		// the section reports the number of children in a 'children_count' node
		const auto& cc =
			tml::const_crawler(f) //
				.to("ruis::container").in().to("children_count").in().get().value;
		tst::check(cc == "3", SL) << cc.string;
	});

	suite.add("get_children_empty", [] {
		auto ctx = make_dummy_context();

		auto c = m::container(ctx, ruis::container::all_parameters{}, ruis::widget_list{});

		const auto resp = c.get().probe(tml::tree(tml::leaf("get_children")));

		const tml::forest f(resp.begin(), resp.end());

		tst::check(f.size() == 1, SL) << f.size();
		tst::check(f[0].value == "ruis::container", SL) << f[0].value.string;
		const auto& cc =
			tml::const_crawler(f) //
				.to("ruis::container").in().to("children_count").in().get().value;
		tst::check(cc == "0", SL) << cc.string;
	});

	suite.add("get_state_delegates_to_base", [] {
		auto ctx = make_dummy_context();

		auto child = m::gap(ctx, ruis::gap::all_parameters{});
		auto c = m::container(ctx, ruis::container::all_parameters{}, ruis::widget_list{child});

		const auto resp = c.get().probe(tml::tree(tml::leaf("get_state")));

		const tml::forest f(resp.begin(), resp.end());

		// container adds no section of its own to the get_state fold,
		// so the response is exactly the base 'ruis::widget' section
		tst::check(f.size() == 1, SL) << f.size();
		tst::check(f[0].value == "ruis::widget", SL) << f[0].value.string;
	});

	suite.add("unknown_command", [] {
		auto ctx = make_dummy_context();

		auto c = m::container(ctx, ruis::container::all_parameters{}, ruis::widget_list{});

		const auto resp = c.get().probe(tml::tree(tml::leaf("frobnicate")));

		tst::check(resp.empty(), SL);
	});
});
} // namespace
