#include <ruis/gui.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/widget.hpp>
#include <tml/crawler.hpp>
#include <tml/tree.hpp>
#include <tst/check.hpp>
#include <tst/set.hpp>

#include "../../harness/util/dummy_context.hpp"

namespace m {
using namespace ruis::make;
} // namespace m

namespace {
const tst::set set("probe", [](tst::suite& suite) {
	// Note: the probe response structure is deterministic, so the tests
	// navigate it with tml::const_crawler cascades - a structural mismatch
	// (regression) fails the test with an exception thrown by the crawler.

	suite.add("unknown_command", [] {
		auto ctx = make_dummy_context();
		auto w = m::gap(ctx, {});

		// command: value is the command name, children are the parameters (none here)
		const tml::tree cmd(tml::leaf("frobnicate"));
		const auto resp = w.get().probe(cmd);

		// unknown commands result in an empty response
		tst::check(resp.empty(), SL);
	});

	suite.add("unknown_command_gui_root", [] {
		ruis::gui gui(make_dummy_context());

		const auto resp = gui.get_root().probe(tml::tree(tml::leaf("frobnicate")));

		tst::check(resp.empty(), SL);
	});

	suite.add("get_state_color_widget", [] {
		auto ctx = make_dummy_context();

		ruis::gap::all_parameters p;
		p.widget.id = "test-gap";
		p.color = ruis::color(1, 2, 3, 255);
		p.layout_params.dims = r4::vector2<ruis::dim>(ruis::dim::min, ruis::dim(ruis::length::make_px(10)));
		p.layout_params.weight = 1.5;
		p.layout_params.align =
			r4::vector2<ruis::styled<ruis::align>>(ruis::align::center, ruis::align::front);
		auto w = m::gap(ctx, p);

		const auto resp = w.get().probe(tml::tree(tml::leaf("get_state")));

		// convert the response to a forest for crawling;
		// sections are sorted by name
		const tml::forest f(resp.begin(), resp.end());

		// fold: two sections
		tst::check(f.size() == 2, SL) << f.size();
		tst::check(f[0].value == "ruis::color_widget", SL) << f[0].value.string;
		tst::check(f[1].value == "ruis::widget", SL) << f[1].value.string;

		// the base widget section
		const auto& idv =
			tml::const_crawler(f)
				.to("ruis::widget") //
				.in() //
				.to("id") //
				.in() //
				.get()
				.value;
		tst::check(idv == "test-gap", SL) << idv.string;

		// the color_widget section,
		// the resolved color is reported as an "rgba" node
		// with the decimal values of the 4 channels as children
		const auto& color_node =
			tml::const_crawler(f)
				.to("ruis::color_widget") //
				.in() //
				.to("color") //
				.in() //
				.get();
		tst::check(color_node.value == "rgba", SL) << color_node.value.string;
		tst::check_eq(color_node.children.size(), std::size_t(4), SL);
		tst::check(color_node.children[0].value == "1", SL) << color_node.children[0].value;
		tst::check(color_node.children[1].value == "2", SL) << color_node.children[1].value;
		tst::check(color_node.children[2].value == "3", SL) << color_node.children[2].value;
		tst::check(color_node.children[3].value == "255", SL) << color_node.children[3].value;

		// the base widget section, layout_params subsection
		const auto& dvx =
			tml::const_crawler(f)
				.to("ruis::widget") //
				.in() //
				.to("layout_params") //
				.in() //
				.to("dims") //
				.in() //
				.to("x") //
				.in() //
				.get()
				.value;
		tst::check(dvx == "min", SL) << dvx.string;

		const auto& dvy =
			tml::const_crawler(f)
				.to("ruis::widget") //
				.in() //
				.to("layout_params") //
				.in() //
				.to("dims") //
				.in() //
				.to("y") //
				.in() //
				.get()
				.value;
		// the length dimension is reported as <value><unit>
		tst::check(dvy == "10px", SL) << dvy.string;

		const auto& wgt =
			tml::const_crawler(f)
				.to("ruis::widget") //
				.in() //
				.to("layout_params") //
				.in() //
				.to("weight") //
				.in() //
				.get()
				.value;
		tst::check(wgt == "1.5", SL) << wgt.string;

		const auto& avx =
			tml::const_crawler(f)
				.to("ruis::widget") //
				.in() //
				.to("layout_params") //
				.in() //
				.to("align") //
				.in() //
				.to("x") //
				.in() //
				.get()
				.value;
		tst::check(avx == "center", SL) << avx.string;

		const auto& avy =
			tml::const_crawler(f)
				.to("ruis::widget") //
				.in() //
				.to("layout_params") //
				.in() //
				.to("align") //
				.in() //
				.to("y") //
				.in() //
				.get()
				.value;
		tst::check(avy == "front", SL) << avy.string;
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

		tst::check(resp.size() == 2, SL) << resp.size();

		const tml::forest f(resp.begin(), resp.end());
		tst::check(f[0].value == "a", SL) << f[0].value.string;
		tst::check(f[1].value == "b", SL) << f[1].value.string;

		const auto& bv =
			tml::const_crawler(f)
				.to("b") //
				.in() //
				.get()
				.value;
		tst::check(bv == "first-b", SL) << bv.string;
	});

	suite.add("unknown_command_color_widget", [] {
		// commands other than 'get_state' result in an empty response
		// even when the widget has a color state
		auto ctx = make_dummy_context();

		ruis::gap::all_parameters p;
		p.color = ruis::color(1, 2, 3, 255);
		auto w = m::gap(ctx, p);

		const auto resp = w.get().probe(tml::tree(tml::leaf("frobnicate")));

		tst::check(resp.empty(), SL);
	});

	suite.add("unknown_command_with_params", [] {
		// a command may carry parameters,
		// unknown commands still result in an empty response
		auto ctx = make_dummy_context();
		auto w = m::gap(ctx, {});

		const tml::tree cmd(
			tml::leaf("frobnicate"), //
			{tml::tree(
				tml::leaf("param"), //
				{tml::tree(tml::leaf("value"))})}
		);
		const auto resp = w.get().probe(cmd);

		tst::check(resp.empty(), SL);
	});
});
} // namespace
