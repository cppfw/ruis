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

		// the response value is always the same as the command name
		tst::check(resp.value == "frobnicate", SL) << resp.value.string;

		// the payload contains an 'error' node with an 'id' leaf
		const auto& idv =
			tml::const_crawler(resp.children)
				.to("error") //
				.in() //
				.in() //
				.get()
				.value;
		tst::check(idv == "unknown_command", SL) << idv.string;

		// error message
		const auto& msgv =
			tml::const_crawler(resp.children)
				.to("error") //
				.in() //
				.to("message") //
				.in() //
				.get()
				.value;
		tst::check(msgv == "Unknown command: frobnicate", SL) << msgv.string;

		// diagnostic widget_class is the most derived class of the probed widget
		const auto& wcv =
			tml::const_crawler(resp.children)
				.to("error") //
				.in() //
				.to("diagnostic") //
				.in() //
				.in() //
				.get()
				.value;
		tst::check(wcv == "ruis::gap", SL) << wcv.string;
	});

	suite.add("unknown_command_gui_root", [] {
		ruis::gui gui(make_dummy_context());

		// the default root widget is a ruis::gap
		const auto resp = gui.get_root().probe(tml::tree(tml::leaf("frobnicate")));

		tst::check(resp.value == "frobnicate", SL);

		// diagnostic widget_class is the most derived class of the probed widget
		const auto& wcv =
			tml::const_crawler(resp.children)
				.to("error") //
				.in() //
				.to("diagnostic") //
				.in() //
				.in() //
				.get()
				.value;
		tst::check(wcv == "ruis::gap", SL) << wcv.string;
	});

	suite.add("unknown_command_with_params", [] {
		// a command may carry parameters,
		// the default implementation still responds with 'unknown_command'
		auto ctx = make_dummy_context();
		auto w = m::gap(ctx, {});

		const tml::tree cmd(
			tml::leaf("frobnicate"), //
			{tml::tree(
				tml::leaf("param"), //
				{tml::tree(tml::leaf("value"))})}
		);
		const auto resp = w.get().probe(cmd);

		tst::check(resp.value == "frobnicate", SL);

		// the payload contains an 'error' node with an 'id' leaf
		const auto& idv =
			tml::const_crawler(resp.children)
				.to("error") //
				.in() //
				.in() //
				.get()
				.value;
		tst::check(idv == "unknown_command", SL) << idv.string;
	});
});
} // namespace
