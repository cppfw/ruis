#include <ruis/gui.hpp>
#include <ruis/widget/container.hpp>
#include <ruis/widget/widget.hpp>
#include <tml/crawler.hpp>
#include <tml/tree.hpp>
#include <tst/check.hpp>
#include <tst/set.hpp>

#include "../../../harness/util/dummy_context.hpp"

namespace {
const tst::set set("widget_probe", [](tst::suite& suite) {
	// Note: the probe response structure is deterministic, so the tests
	// navigate it with tml::const_crawler cascades - a structural mismatch
	// (regression) fails the test with an exception thrown by the crawler.
	//
	// The tests use ruis::container as the test vehicle: a container is a
	// plain widget (it has no color state and does not override probe()),
	// so its probe response consists of the single 'ruis::widget' section
	// produced by widget::probe() alone.

	suite.add("unknown_command", [] {
		auto ctx = make_dummy_context();
		auto c = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list());

		// command: value is the command name, children are the parameters (none here)
		const tml::tree cmd(tml::leaf("frobnicate"));
		const auto resp = c.get().probe(cmd);

		// unknown commands result in an empty response
		tst::check(resp.empty(), SL);
	});

	suite.add("unknown_command_with_params", [] {
		// a command may carry parameters,
		// unknown commands still result in an empty response
		auto ctx = make_dummy_context();
		auto c = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list());

		const tml::tree cmd(
			tml::leaf("frobnicate"), //
			{tml::tree(
				tml::leaf("param"), //
				{tml::tree(tml::leaf("value"))}
			)}
		);
		const auto resp = c.get().probe(cmd);

		tst::check(resp.empty(), SL);
	});

	suite.add("unknown_command_gui_root", [] {
		ruis::gui gui(make_dummy_context());

		const auto resp = gui.get_root().probe(tml::tree(tml::leaf("frobnicate")));

		tst::check(resp.empty(), SL);
	});

	suite.add("get_state", [] {
		auto ctx = make_dummy_context();

		ruis::container::all_parameters p;
		p.widget.id = "test-container";
		p.layout_params.dims = r4::vector2<ruis::dim>(ruis::dim::min, ruis::dim(ruis::length::make_px(10)));
		p.layout_params.weight = 1.5;
		p.layout_params.align = r4::vector2<ruis::styled<ruis::align>>(ruis::align::center, ruis::align::front);
		auto c = utki::make_shared<ruis::container>(ctx, p, ruis::widget_list());

		const auto resp = c.get().probe(tml::tree(tml::leaf("get_state")));

		// convert the response to a forest for crawling;
		// sections are sorted by name
		const tml::forest f(resp.begin(), resp.end());

		// a plain widget reports a single section named after its class
		tst::check(
			f.size() == 1,
			[&](auto& o) {
				o << f.size();
			},
			SL
		);
		tst::check(
			f[0].value == "ruis::widget",
			[&](auto& o) {
				o << f[0].value.string;
			},
			SL
		);

		// the id
		const auto& idv = tml::const_crawler(f)
							  .to("ruis::widget") //
							  .in() //
							  .to("id") //
							  .in() //
							  .get()
							  .value;
		tst::check(
			idv == "test-container",
			[&](auto& o) {
				o << idv.string;
			},
			SL
		);

		// the layout_params subsection
		const auto& dvx = tml::const_crawler(f)
							  .to("ruis::widget") //
							  .in() //
							  .to("layout_params") //
							  .in() //
							  .to("dims") //
							  .in() //
							  .get()
							  .value;
		tst::check(
			dvx == "min",
			[&](auto& o) {
				o << dvx.string;
			},
			SL
		);

		const auto& dvy = tml::const_crawler(f)
							  .to("ruis::widget") //
							  .in() //
							  .to("layout_params") //
							  .in() //
							  .to("dims") //
							  .in() //
							  .next() //
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

		const auto& wgt = tml::const_crawler(f)
							  .to("ruis::widget") //
							  .in() //
							  .to("layout_params") //
							  .in() //
							  .to("weight") //
							  .in() //
							  .get()
							  .value;
		tst::check(
			wgt == "1.5",
			[&](auto& o) {
				o << wgt.string;
			},
			SL
		);

		const auto& avx = tml::const_crawler(f)
							  .to("ruis::widget") //
							  .in() //
							  .to("layout_params") //
							  .in() //
							  .to("align") //
							  .in() //
							  .get()
							  .value;
		tst::check(
			avx == "center",
			[&](auto& o) {
				o << avx.string;
			},
			SL
		);

		const auto& avy = tml::const_crawler(f)
							  .to("ruis::widget") //
							  .in() //
							  .to("layout_params") //
							  .in() //
							  .to("align") //
							  .in() //
							  .next() //
							  .get()
							  .value;
		tst::check(
			avy == "front",
			[&](auto& o) {
				o << avy.string;
			},
			SL
		);

		// the widget's rectangle:
		// default position is (0, 0) and default size is (-1, -1)
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
		const auto& py = tml::const_crawler(f) //
							 .to("ruis::widget")
							 .in()
							 .to("rect")
							 .in()
							 .to("p")
							 .in()
							 .next()
							 .get()
							 .value;
		tst::check(
			py == "0",
			[&](auto& o) {
				o << py.string;
			},
			SL
		);
		const auto& dx = tml::const_crawler(f) //
							 .to("ruis::widget")
							 .in()
							 .to("rect")
							 .in()
							 .to("d")
							 .in()
							 .get()
							 .value;
		tst::check(
			dx == "-1",
			[&](auto& o) {
				o << dx.string;
			},
			SL
		);
		const auto& dy = tml::const_crawler(f) //
							 .to("ruis::widget")
							 .in()
							 .to("rect")
							 .in()
							 .to("d")
							 .in()
							 .next()
							 .get()
							 .value;
		tst::check(
			dy == "-1",
			[&](auto& o) {
				o << dy.string;
			},
			SL
		);

		// scalar state flags of a freshly created widget
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

		const auto& vis = tml::const_crawler(f) //
							  .to("ruis::widget")
							  .in()
							  .to("visible")
							  .in()
							  .get()
							  .value;
		tst::check(
			vis == "true",
			[&](auto& o) {
				o << vis.string;
			},
			SL
		); // default visible

		const auto& en = tml::const_crawler(f) //
							 .to("ruis::widget")
							 .in()
							 .to("enabled")
							 .in()
							 .get()
							 .value;
		tst::check(
			en == "true",
			[&](auto& o) {
				o << en.string;
			},
			SL
		); // default enabled

		const auto& fo = tml::const_crawler(f) //
							 .to("ruis::widget")
							 .in()
							 .to("focused")
							 .in()
							 .get()
							 .value;
		tst::check(
			fo == "false",
			[&](auto& o) {
				o << fo.string;
			},
			SL
		);

		// the render cache: disabled, dirty and not built for a new widget
		const auto& cbe = tml::const_crawler(f) //
							  .to("ruis::widget")
							  .in()
							  .to("cache")
							  .in()
							  .to("enabled")
							  .in()
							  .get()
							  .value;
		tst::check(
			cbe == "false",
			[&](auto& o) {
				o << cbe.string;
			},
			SL
		);
		const auto& cbd = tml::const_crawler(f) //
							  .to("ruis::widget")
							  .in()
							  .to("cache")
							  .in()
							  .to("dirty")
							  .in()
							  .get()
							  .value;
		tst::check(
			cbd == "true",
			[&](auto& o) {
				o << cbd.string;
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

	suite.add("get_abs_rect", [] {
		auto ctx = make_dummy_context();
		auto c = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list());

		const auto resp = c.get().probe(tml::tree(tml::leaf("get_abs_rect")));

		const tml::forest f(resp.begin(), resp.end());

		// a single 'ruis::widget' section carrying the 'abs_rect' field
		tst::check(
			f.size() == 1,
			[&](auto& o) {
				o << f.size();
			},
			SL
		);
		tst::check(
			f[0].value == "ruis::widget",
			[&](auto& o) {
				o << f[0].value.string;
			},
			SL
		);

		// a fresh standalone widget: the absolute rect is at (0,0) with size (-1,-1)
		const auto& apx = tml::const_crawler(f) //
							 .to("ruis::widget") //
							 .in() //
							 .to("abs_rect") //
							 .in() //
							 .to("p") //
							 .in() //
							 .get()
							 .value;
		tst::check(
			apx == "0",
			[&](auto& o) {
				o << apx.string;
			},
			SL
		);
		const auto& adx = tml::const_crawler(f) //
							 .to("ruis::widget") //
							 .in() //
							 .to("abs_rect") //
							 .in() //
							 .to("d") //
							 .in() //
							 .get()
							 .value;
		tst::check(
			adx == "-1",
			[&](auto& o) {
				o << adx.string;
			},
			SL
		);
	});

	suite.add("response_section_duplicates", [] {
		// sections are identified by name: inserting a section with a name
		// already present in the response is a no-op, even if the content
		// differs, the first inserted section is kept
		ruis::widget::probe_response_set resp;
		resp.insert(tml::tree(
			tml::leaf("b"), //
			{tml::tree(tml::leaf("first-b"))}
		));
		resp.insert(tml::tree(
			tml::leaf("a"), //
			{tml::tree(tml::leaf("only-a"))}
		));
		resp.insert(tml::tree(
			tml::leaf("b"), //
			{tml::tree(tml::leaf("second-b"))}
		));

		tst::check(
			resp.size() == 2,
			[&](auto& o) {
				o << resp.size();
			},
			SL
		);

		const tml::forest f(resp.begin(), resp.end());
		tst::check(
			f[0].value == "a",
			[&](auto& o) {
				o << f[0].value.string;
			},
			SL
		);
		tst::check(
			f[1].value == "b",
			[&](auto& o) {
				o << f[1].value.string;
			},
			SL
		);

		const auto& bv = tml::const_crawler(f)
							 .to("b") //
							 .in() //
							 .get()
							 .value;
		tst::check(
			bv == "first-b",
			[&](auto& o) {
				o << bv.string;
			},
			SL
		);
	});
});
} // namespace
