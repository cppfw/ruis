#include <fsif/vector_file.hpp>
#include <rasterimage/image_variant.hpp>
#include <ruis/gui.hpp>
#include <ruis/widget/container.hpp>
#include <ruis/widget/widget.hpp>
#include <tml/crawler.hpp>
#include <tml/tree.hpp>
#include <tst/check.hpp>
#include <tst/set.hpp>
#include <utki/base64.hpp>

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
		// note: ruis::dim::min (the static member) cannot be referenced here because
		// the global test set would then depend on a dynamically initialized non-local
		// variable (clang-tidy cppcoreguidelines-interfaces-global-init)
		p.layout_params.dims = {ruis::dim(ruis::dim::type::min), ruis::length::make_px(10)};
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

		// a single 'ruis::widget' section carrying the rect's component nodes (p, d) directly
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

		// a fresh standalone widget: the absolute rect is at (0,0) with size (-1,-1); the
		// rectangle's component nodes (p, d) are direct children of the section
		const auto& apx = tml::const_crawler(f) //
							  .to("ruis::widget") //
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

	suite.add("get_index", [] {
		auto ctx = make_dummy_context();

		// build the hierarchy:
		//   root
		//     [0] c0
		//     [1] c1
		//          [0] t0
		//          [1] t1
		//          [2] target
		//     [2] c2
		// the index path of 'target' (root first) is [1, 2]: c1 is at index 1
		// in root, target is at index 2 in c1
		auto t0 = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list());
		auto t1 = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list());
		auto target = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list());
		auto c1 = utki::make_shared<ruis::container>(
			ctx,
			ruis::container::all_parameters(),
			ruis::widget_list{t0, t1, target}
		);
		auto c0 = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list());
		auto c2 = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list());
		auto root =
			utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list{c0, c1, c2});

		const auto resp = target.get().probe(tml::tree(tml::leaf("get_index")));

		const tml::forest f(resp.begin(), resp.end());

		// a single 'ruis::widget' section carrying the index-path components directly
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

		// first component: the index of c1 in root = 1
		const auto& first = tml::const_crawler(f) //
								.to("ruis::widget") //
								.in() //
								.get()
								.value;
		tst::check(
			first == "1",
			[&](auto& o) {
				o << first.string;
			},
			SL
		);

		// second component: the index of target in c1 = 2
		const auto& second = tml::const_crawler(f) //
								 .to("ruis::widget") //
								 .in() //
								 .next() //
								 .get()
								 .value;
		tst::check(
			second == "2",
			[&](auto& o) {
				o << second.string;
			},
			SL
		);

		// the root widget has an empty index path
		const auto root_resp = root.get().probe(tml::tree(tml::leaf("get_index")));
		const tml::forest rf(root_resp.begin(), root_resp.end());
		const auto& ridx = tml::const_crawler(rf) //
							   .to("ruis::widget") //
							   .get();
		tst::check(
			ridx.children.empty(),
			[&](auto& o) {
				o << ridx.children.size();
			},
			SL
		);
	});

	suite.add("get_render", [] {
		// the render target size, in pixels
		constexpr uint32_t w = 8;
		constexpr uint32_t h = 6;

		auto ctx = make_dummy_context();
		auto c = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list());
		// give the widget a defined non-zero size, so the render-to-texture FBO has that size
		c.get().resize(ruis::vec2(w, h));

		const auto resp = c.get().probe(tml::tree(tml::leaf("get_render")));

		// a single 'ruis::widget' section carrying the base64 PNG directly
		const tml::forest f(resp.begin(), resp.end());
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

		// the base64-encoded PNG bytes
		const auto& b64 = tml::const_crawler(f) //
							  .to("ruis::widget") //
							  .in() //
							  .get()
							  .value;

		// decode the base64 back to the PNG bytes
		const auto png = utki::base64_decode(b64.string);
		tst::check(
			!png.empty(),
			[&](auto& o) {
				o << png.size();
			},
			SL
		);

		// round-trip: decode the PNG (read_png throws if the bytes are not a valid PNG)
		// and check that the dimensions match the widget size
		fsif::vector_file fi;
		{
			fsif::file::guard guard(fi, fsif::mode::write);
			fi.write(utki::make_span(png));
		}
		const auto im = rasterimage::read_png(fi);
		tst::check(
			im.dims().x() == w && im.dims().y() == h,
			[&](auto& o) {
				o << im.dims().x() << "x" << im.dims().y();
			},
			SL
		);
	});
});
} // namespace
