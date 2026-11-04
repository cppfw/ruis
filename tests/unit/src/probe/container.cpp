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

// build a 'for' probe command:
// for { index { <indices...> } <command> }
tml::tree make_for_cmd(std::initializer_list<int> indices, const char* command)
{
	tml::forest index_nodes;
	for (auto i : indices) {
		index_nodes.push_back(tml::tree(tml::leaf(i)));
	}
	return tml::tree(
		tml::leaf("for"), //
		{tml::tree(tml::leaf("index"), std::move(index_nodes)), tml::tree(tml::leaf(command))}
	);
}

// build a 'for' probe command with string index values
// (for indices which do not fit in int)
tml::tree make_for_cmd_from_strings(std::initializer_list<const char*> indices, const char* command)
{
	tml::forest index_nodes;
	for (auto i : indices) {
		index_nodes.push_back(tml::tree(tml::leaf(i)));
	}
	return tml::tree(
		tml::leaf("for"), //
		{tml::tree(tml::leaf("index"), std::move(index_nodes)), tml::tree(tml::leaf(command))}
	);
}

// check that the response is the malformed command error:
// ruis::container { error { malformed_command } }
void check_malformed_command_response(const ruis::widget::probe_response_set& resp)
{
	const tml::forest f(resp.begin(), resp.end());
	tst::check(
		f.size() == 1,
		[&](auto& o) {
			o << f.size();
		},
		SL
	);
	tst::check(
		f[0].value == "ruis::container",
		[&](auto& o) {
			o << f[0].value.string;
		},
		SL
	);
	const auto& err = tml::const_crawler(f).to("ruis::container").in().to("error").get();
	tst::check_eq(err.children.size(), std::size_t(1), SL);
	tst::check(
		err.children[0].value == "malformed_command",
		[&](auto& o) {
			o << err.children[0].value.string;
		},
		SL
	);
	tst::check(err.children[0].children.empty(), SL);
}

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
		auto c = m::container(ctx, ruis::container::all_parameters{}, ruis::widget_list{child1, child2, child3});

		const auto resp = c.get().probe(tml::tree(tml::leaf("get_children")));

		const tml::forest f(resp.begin(), resp.end());

		// a non-fold command: a single section named after the container class
		tst::check(
			f.size() == 1,
			[&](auto& o) {
				o << f.size();
			},
			SL
		);
		tst::check(
			f[0].value == "ruis::container",
			[&](auto& o) {
				o << f[0].value.string;
			},
			SL
		);

		// the section reports the number of children in a 'children_count' node
		const auto& cc = tml::const_crawler(f) //
							 .to("ruis::container")
							 .in()
							 .to("children_count")
							 .in()
							 .get()
							 .value;
		tst::check(
			cc == "3",
			[&](auto& o) {
				o << cc.string;
			},
			SL
		);
	});

	suite.add("get_children_empty", [] {
		auto ctx = make_dummy_context();

		auto c = m::container(ctx, ruis::container::all_parameters{}, ruis::widget_list{});

		const auto resp = c.get().probe(tml::tree(tml::leaf("get_children")));

		const tml::forest f(resp.begin(), resp.end());

		tst::check(
			f.size() == 1,
			[&](auto& o) {
				o << f.size();
			},
			SL
		);
		tst::check(
			f[0].value == "ruis::container",
			[&](auto& o) {
				o << f[0].value.string;
			},
			SL
		);
		const auto& cc = tml::const_crawler(f) //
							 .to("ruis::container")
							 .in()
							 .to("children_count")
							 .in()
							 .get()
							 .value;
		tst::check(
			cc == "0",
			[&](auto& o) {
				o << cc.string;
			},
			SL
		);
	});

	suite.add("get_state_delegates_to_base", [] {
		auto ctx = make_dummy_context();

		auto child = m::gap(ctx, ruis::gap::all_parameters{});
		auto c = m::container(ctx, ruis::container::all_parameters{}, ruis::widget_list{child});

		const auto resp = c.get().probe(tml::tree(tml::leaf("get_state")));

		const tml::forest f(resp.begin(), resp.end());

		// container adds no section of its own to the get_state fold,
		// so the response is exactly the base 'ruis::widget' section
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
	});

	suite.add("unknown_command", [] {
		auto ctx = make_dummy_context();

		auto c = m::container(ctx, ruis::container::all_parameters{}, ruis::widget_list{});

		const auto resp = c.get().probe(tml::tree(tml::leaf("frobnicate")));

		tst::check(resp.empty(), SL);
	});

	suite.add("for_runs_command_on_target", [] {
		auto ctx = make_dummy_context();

		// c
		//   [0] gap
		//   [1] gap
		//   [2] container
		//        [0] gap
		//        [1] gap
		auto g20 = m::gap(ctx, ruis::gap::all_parameters{});
		auto g21 = m::gap(ctx, ruis::gap::all_parameters{});
		auto c2 = m::container(
			ctx, //
			ruis::container::all_parameters{}, //
			ruis::widget_list{g20, g21}
		);
		auto g0 = m::gap(ctx, ruis::gap::all_parameters{});
		auto g1 = m::gap(ctx, ruis::gap::all_parameters{});
		auto c = m::container(
			ctx, //
			ruis::container::all_parameters{}, //
			ruis::widget_list{g0, g1, c2}
		);

		// run 'get_state' on the [1]st child of the [2]nd child of c (a gap):
		// the response is exactly the probe response of that widget
		const auto resp = c.get().probe(make_for_cmd({2, 1}, "get_state"));
		const tml::forest f(resp.begin(), resp.end());

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
	});

	suite.add("for_empty_index", [] {
		auto ctx = make_dummy_context();

		auto c = m::container(ctx, ruis::container::all_parameters{}, ruis::widget_list{});

		// an empty index path runs the command on this container itself
		tml::tree cmd(
			tml::leaf("for"), //
			{tml::tree(tml::leaf("index")), tml::tree(tml::leaf("get_children"))}
		);
		const auto resp = c.get().probe(cmd);
		const tml::forest f(resp.begin(), resp.end());

		tst::check(
			f.size() == 1,
			[&](auto& o) {
				o << f.size();
			},
			SL
		);
		const auto& cc = tml::const_crawler(f) //
							 .to("ruis::container")
							 .in()
							 .to("children_count")
							 .in()
							 .get()
							 .value;
		tst::check(
			cc == "0",
			[&](auto& o) {
				o << cc.string;
			},
			SL
		);
	});

	suite.add("for_out_of_bound", [] {
		auto ctx = make_dummy_context();

		auto child = m::gap(ctx, ruis::gap::all_parameters{});
		auto c2 = m::container(ctx, ruis::container::all_parameters{}, ruis::widget_list{child});
		auto c = m::container(ctx, ruis::container::all_parameters{}, ruis::widget_list{c2});

		// the [0]th child of c has 1 child; the path [0 5] is out of bound
		// at the last step
		const auto resp = c.get().probe(make_for_cmd({0, 5}, "get_state"));
		const tml::forest f(resp.begin(), resp.end());

		tst::check(
			f.size() == 1,
			[&](auto& o) {
				o << f.size();
			},
			SL
		);
		tst::check(
			f[0].value == "ruis::container",
			[&](auto& o) {
				o << f[0].value.string;
			},
			SL
		);

		// error { index { 0 5 } out_of_bound 1 }
		const auto& err = tml::const_crawler(f).to("ruis::container").in().to("error").get();
		tst::check_eq(err.children.size(), std::size_t(3), SL);
		tst::check(
			err.children[0].value == "index",
			[&](auto& o) {
				o << err.children[0].value.string;
			},
			SL
		);
		tst::check_eq(err.children[0].children.size(), std::size_t(2), SL);
		tst::check(
			err.children[0].children[0].value == "0",
			[&](auto& o) {
				o << err.children[0].children[0].value.string;
			},
			SL
		);
		tst::check(
			err.children[0].children[1].value == "5",
			[&](auto& o) {
				o << err.children[0].children[1].value.string;
			},
			SL
		);
		tst::check(
			err.children[1].value == "out_of_bound",
			[&](auto& o) {
				o << err.children[1].value.string;
			},
			SL
		);
		tst::check(
			err.children[2].value == "1",
			[&](auto& o) {
				o << err.children[2].value.string;
			},
			SL
		);
	});

	suite.add("for_not_a_container", [] {
		auto ctx = make_dummy_context();

		auto g0 = m::gap(ctx, ruis::gap::all_parameters{});
		auto c = m::container(ctx, ruis::container::all_parameters{}, ruis::widget_list{g0});

		// the [0]th child of c is a gap, not a container;
		// the path [0 3] fails at the last step
		const auto resp = c.get().probe(make_for_cmd({0, 3}, "get_state"));
		const tml::forest f(resp.begin(), resp.end());

		tst::check(
			f.size() == 1,
			[&](auto& o) {
				o << f.size();
			},
			SL
		);
		tst::check(
			f[0].value == "ruis::container",
			[&](auto& o) {
				o << f[0].value.string;
			},
			SL
		);

		// error { index { 0 3 } not_a_container }
		const auto& err = tml::const_crawler(f).to("ruis::container").in().to("error").get();
		tst::check_eq(err.children.size(), std::size_t(2), SL);
		tst::check(
			err.children[0].value == "index",
			[&](auto& o) {
				o << err.children[0].value.string;
			},
			SL
		);
		tst::check_eq(err.children[0].children.size(), std::size_t(2), SL);
		tst::check(
			err.children[0].children[0].value == "0",
			[&](auto& o) {
				o << err.children[0].children[0].value.string;
			},
			SL
		);
		tst::check(
			err.children[0].children[1].value == "3",
			[&](auto& o) {
				o << err.children[0].children[1].value.string;
			},
			SL
		);
		tst::check(
			err.children[1].value == "not_a_container",
			[&](auto& o) {
				o << err.children[1].value.string;
			},
			SL
		);
	});

	suite.add("for_malformed", [] {
		auto ctx = make_dummy_context();

		auto c = m::container(ctx, ruis::container::all_parameters{}, ruis::widget_list{});

		// two commands
		{
			tml::tree cmd(
				tml::leaf("for"), //
				{tml::tree(tml::leaf("index")), tml::tree(tml::leaf("get_state")), tml::tree(tml::leaf("get_children"))}
			);
			check_malformed_command_response(c.get().probe(cmd));
		}

		// no command
		{
			tml::tree cmd(tml::leaf("for"), {tml::tree(tml::leaf("index"))});
			check_malformed_command_response(c.get().probe(cmd));
		}

		// non-numeric index
		{
			tml::tree cmd(
				tml::leaf("for"), //
				{tml::tree(tml::leaf("index"), {tml::tree(tml::leaf("x"))}), tml::tree(tml::leaf("get_state"))}
			);
			check_malformed_command_response(c.get().probe(cmd));
		}

		// an index that overflows uint64_t cannot be parsed and is malformed
		{
			tml::tree cmd(
				tml::leaf("for"), //
				{tml::tree(tml::leaf("index"), {tml::tree(tml::leaf("18446744073709551616"))}), tml::tree(tml::leaf("get_state"))}
			);
			check_malformed_command_response(c.get().probe(cmd));
		}
	});

	suite.add("for_too_big_index", [] {
		auto ctx = make_dummy_context();

		auto child = m::gap(ctx, ruis::gap::all_parameters{});
		auto c = m::container(ctx, ruis::container::all_parameters{}, ruis::widget_list{child});

		// an index that fits in uint64_t but exceeds the number of children
		// is out of bound
		const auto resp = c.get().probe(make_for_cmd_from_strings({"12345678901234567890"}, "get_state"));
		const tml::forest f(resp.begin(), resp.end());

		tst::check(
			f.size() == 1,
			[&](auto& o) {
				o << f.size();
			},
			SL
		);
		tst::check(
			f[0].value == "ruis::container",
			[&](auto& o) {
				o << f[0].value.string;
			},
			SL
		);
		const auto& err = tml::const_crawler(f).to("ruis::container").in().to("error").get();
		tst::check_eq(err.children.size(), std::size_t(3), SL);
		tst::check(
			err.children[0].value == "index",
			[&](auto& o) {
				o << err.children[0].value.string;
			},
			SL
		);
		tst::check_eq(err.children[0].children.size(), std::size_t(1), SL);
		tst::check(
			err.children[0].children[0].value == "12345678901234567890",
			[&](auto& o) {
				o << err.children[0].children[0].value.string;
			},
			SL
		);
		tst::check(
			err.children[1].value == "out_of_bound",
			[&](auto& o) {
				o << err.children[1].value.string;
			},
			SL
		);
		tst::check(
			err.children[2].value == "1",
			[&](auto& o) {
				o << err.children[2].value.string;
			},
			SL
		);
	});
});
} // namespace
