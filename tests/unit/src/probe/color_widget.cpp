#include <ruis/style/styled.hpp>
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
const tst::set set("color_widget_probe", [](tst::suite& suite) {
	// Note: the probe response structure is deterministic, so the tests
	// navigate it with tml::const_crawler cascades - a structural mismatch
	// (regression) fails the test with an exception thrown by the crawler.
	//
	// The tests use ruis::gap: it is a color widget that does not override
	// probe() itself, so its probe response is the fold of the
	// 'ruis::color_widget' section (added by color_widget::probe()) on top
	// of the base 'ruis::widget' section (produced by widget::probe()).

	suite.add("get_state", [] {
		auto ctx = make_dummy_context();

		ruis::gap::all_parameters p;
		p.widget.id = "test-gap";
		p.color = ruis::color(1, 2, 3, 255);
		p.layout_params.dims = r4::vector2<ruis::dim>(ruis::dim::min, ruis::dim(ruis::length::make_px(10)));
		auto w = m::gap(ctx, p);

		const auto resp = w.get().probe(tml::tree(tml::leaf("get_state")));

		// convert the response to a forest for crawling;
		// sections are sorted by name
		const tml::forest f(resp.begin(), resp.end());

		// fold: two sections
		tst::check(
			f.size() == 2,
			[&](auto& o) {
				o << f.size();
			},
			SL
		);
		tst::check(
			f[0].value == "ruis::color_widget",
			[&](auto& o) {
				o << f[0].value.string;
			},
			SL
		);
		tst::check(
			f[1].value == "ruis::widget",
			[&](auto& o) {
				o << f[1].value.string;
			},
			SL
		);

		// the color_widget section:
		// the resolved color is reported as an "rgba" node
		// with the decimal values of the 4 channels as children
		const auto& color_node = tml::const_crawler(f)
									 .to("ruis::color_widget") //
									 .in() //
									 .to("color") //
									 .in() //
									 .get();
		tst::check(
			color_node.value == "rgba",
			[&](auto& o) {
				o << color_node.value.string;
			},
			SL
		);
		tst::check_eq(color_node.children.size(), std::size_t(4), SL);
		tst::check(
			color_node.children[0].value == "1",
			[&](auto& o) {
				o << color_node.children[0].value;
			},
			SL
		);
		tst::check(
			color_node.children[1].value == "2",
			[&](auto& o) {
				o << color_node.children[1].value;
			},
			SL
		);
		tst::check(
			color_node.children[2].value == "3",
			[&](auto& o) {
				o << color_node.children[2].value;
			},
			SL
		);
		tst::check(
			color_node.children[3].value == "255",
			[&](auto& o) {
				o << color_node.children[3].value;
			},
			SL
		);

		// the base 'ruis::widget' section is also present and reports the
		// full widget state (the fold started from the base class result)

		// the section contains all the widget state fields
		const auto& wsec = tml::const_crawler(f)
							   .to("ruis::widget") //
							   .get();
		tst::check_eq(
			wsec.children.size(),
			std::size_t(10),
			[&](auto& o) {
				o << wsec.children.size();
			},
			SL
		);

		const auto& idv = tml::const_crawler(f) //
							  .to("ruis::widget")
							  .in()
							  .to("id")
							  .in()
							  .get()
							  .value;
		tst::check(
			idv == "test-gap",
			[&](auto& o) {
				o << idv.string;
			},
			SL
		);

		const auto& dvx = tml::const_crawler(f) //
							  .to("ruis::widget")
							  .in()
							  .to("layout_params")
							  .in()
							  .to("dims")
							  .in()
							  .get()
							  .value;
		tst::check(
			dvx == "min",
			[&](auto& o) {
				o << dvx.string;
			},
			SL
		);

		const auto& dvy = tml::const_crawler(f) //
							  .to("ruis::widget")
							  .in()
							  .to("layout_params")
							  .in()
							  .to("dims")
							  .in()
							  .next()
							  .get()
							  .value;
		// the length dimension is reported as <value><unit>
		tst::check(
			dvy == "10px",
			[&](auto& o) {
				o << dvy.string;
			},
			SL
		);

		const auto& px = tml::const_crawler(f) //
							 .to("ruis::widget")
							 .in()
							 .to("rect")
							 .in()
							 .to("p")
							 .in()
							 .get()
							 .value;
		tst::check(
			px == "0",
			[&](auto& o) {
				o << px.string;
			},
			SL
		);

		const auto& ld = tml::const_crawler(f) //
							 .to("ruis::widget")
							 .in()
							 .to("layout_dirty")
							 .in()
							 .get()
							 .value;
		// a new widget is layout-dirty until the first layout pass
		tst::check(
			ld == "true",
			[&](auto& o) {
				o << ld.string;
			},
			SL
		);

		const auto& cbp = tml::const_crawler(f) //
							  .to("ruis::widget")
							  .in()
							  .to("cache")
							  .in()
							  .to("present")
							  .in()
							  .get()
							  .value;
		tst::check(
			cbp == "false",
			[&](auto& o) {
				o << cbp.string;
			},
			SL
		);
	});

	suite.add("get_state_style_sourced_color", [] {
		auto ctx = make_dummy_context();

		// a style-sourced color: the styled value holds a reference to a
		// style value (in real use it comes from the style provider)
		using sv_type = ruis::internal::style_value<ruis::color>;
		auto sv = std::make_shared<sv_type>(ruis::color(9, 8, 7, 6));
		auto svr = utki::shared_ref<const sv_type>(sv);

		ruis::gap::all_parameters p;
		p.color = ruis::styled<ruis::color>(svr);
		auto w = m::gap(ctx, p);

		const auto resp = w.get().probe(tml::tree(tml::leaf("get_state")));

		const tml::forest f(resp.begin(), resp.end());

		// a style-sourced color is wrapped in a 'style_ref' node
		// which contains the color value tree
		const auto& sr = tml::const_crawler(f)
							 .to("ruis::color_widget") //
							 .in() //
							 .to("color") //
							 .in() //
							 .get();
		tst::check(
			sr.value == "style_ref",
			[&](auto& o) {
				o << sr.value.string;
			},
			SL
		);
		tst::check_eq(sr.children.size(), std::size_t(1), SL);
		const auto& rgba = sr.children[0];
		tst::check(
			rgba.value == "rgba",
			[&](auto& o) {
				o << rgba.value.string;
			},
			SL
		);
		tst::check_eq(rgba.children.size(), std::size_t(4), SL);
		tst::check(
			rgba.children[0].value == "9",
			[&](auto& o) {
				o << rgba.children[0].value;
			},
			SL
		);
		tst::check(
			rgba.children[1].value == "8",
			[&](auto& o) {
				o << rgba.children[1].value;
			},
			SL
		);
		tst::check(
			rgba.children[2].value == "7",
			[&](auto& o) {
				o << rgba.children[2].value;
			},
			SL
		);
		tst::check(
			rgba.children[3].value == "6",
			[&](auto& o) {
				o << rgba.children[3].value;
			},
			SL
		);
	});

	suite.add("unknown_command", [] {
		// commands other than 'get_state' result in an empty response
		// even when the widget has a color state
		auto ctx = make_dummy_context();

		ruis::gap::all_parameters p;
		p.color = ruis::color(1, 2, 3, 255);
		auto w = m::gap(ctx, p);

		const auto resp = w.get().probe(tml::tree(tml::leaf("frobnicate")));

		tst::check(resp.empty(), SL);
	});
});
} // namespace
