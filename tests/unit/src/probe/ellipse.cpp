#include <ruis/widget/label/ellipse.hpp>
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
const tst::set set("ellipse_probe", [](tst::suite& suite) {
	// The ellipse is a padding (container) which is not a color_widget anymore,
	// so its probe responses are exactly those of the container/widget base
	// classes.
	//
	// The explicit border lengths below keep the constructor from resolving
	// undefined borders through the (empty) style sheet of the dummy context.
	auto make_ellipse = [](auto ctx, ruis::widget_list children) {
		ruis::ellipse::all_parameters p;
		for (auto& b : p.params.padding.specific.borders) {
			b = ruis::length::make_px(0);
		}
		return m::ellipse(ctx, std::move(p), std::move(children));
	};

	suite.add("get_children", [&] {
		auto ctx = make_dummy_context();

		auto child = m::gap(ctx, ruis::gap::all_parameters{});
		auto c = make_ellipse(ctx, ruis::widget_list{child});

		// probe() is only public through the ruis::widget base (padding inherits
		// container as a protected base), so cast to the widget reference
		auto& w = static_cast<ruis::widget&>(c.get());
		const auto resp = w.probe(tml::tree(tml::leaf("get_children")));

		const tml::forest f(resp.begin(), resp.end());

		// the response is the single container section
		tst::check(f.size() == 1, SL) << f.size();
		tst::check(f[0].value == "ruis::container", SL) << f[0].value.string;
		const auto& cc = tml::const_crawler(f) //
							 .to("ruis::container")
							 .in()
							 .to("children_count")
							 .in()
							 .get()
							 .value;
		tst::check(cc == "1", SL) << cc.string;
	});

	suite.add("get_state", [] {
		auto ctx = make_dummy_context();

		ruis::ellipse::all_parameters p;
		for (auto& b : p.params.padding.specific.borders) {
			b = ruis::length::make_px(0);
		}
		p.params.specific.fill_color = ruis::color(4, 3, 2, 1);
		auto c = m::ellipse(ctx, std::move(p), ruis::widget_list{});

		// probe() is only public through the ruis::widget base (padding inherits
		// container as a protected base), so cast to the widget reference
		auto& w = static_cast<ruis::widget&>(c.get());
		const auto resp = w.probe(tml::tree(tml::leaf("get_state")));

		const tml::forest f(resp.begin(), resp.end());

		// the response is the single widget section
		tst::check(f.size() == 1, SL) << f.size();
		tst::check(f[0].value == "ruis::widget", SL) << f[0].value.string;
	});
});
} // namespace
