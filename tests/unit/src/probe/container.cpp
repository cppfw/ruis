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

// build a 'for' probe command with an 'id' addressing node:
// for { id { <ids...> } <command> }
tml::tree make_for_id_cmd(std::initializer_list<const char*> ids, const char* command)
{
	tml::forest id_nodes;
	for (auto i : ids) {
		id_nodes.push_back(tml::tree(tml::leaf(i)));
	}
	return tml::tree(
		tml::leaf("for"), //
		{tml::tree(tml::leaf("id"), std::move(id_nodes)), tml::tree(tml::leaf(command))}
	);
}

// build a 'for' probe command with an empty 'id' addressing node:
// for { id { } <command> }
tml::tree make_for_id_cmd_empty(const char* command)
{
	return tml::tree(
		tml::leaf("for"), //
		{tml::tree(tml::leaf("id")), tml::tree(tml::leaf(command))}
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

		// both an 'index' and an 'id' node
		{
			tml::tree cmd(
				tml::leaf("for"), //
				{
					tml::tree(tml::leaf("index"), {tml::tree(tml::leaf("0"))}),
					tml::tree(tml::leaf("id"), {tml::tree(tml::leaf("a"))}),
					tml::tree(tml::leaf("get_state"))
				}
			);
			check_malformed_command_response(c.get().probe(cmd));
		}

		// no addressing node (a command but neither 'index' nor 'id')
		{
			tml::tree cmd(tml::leaf("for"), {tml::tree(tml::leaf("get_state"))});
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

	suite.add("for_id_runs_command_on_target", [] {
		auto ctx = make_dummy_context();

		// c
		//   [0] container id="c2"
		//        [0] gap id="g20"
		//        [1] gap id="g21"
		ruis::gap::all_parameters g20p;
		g20p.widget.id = "g20";
		ruis::gap::all_parameters g21p;
		g21p.widget.id = "g21";
		ruis::container::all_parameters c2p;
		c2p.widget.id = "c2";
		auto g20 = m::gap(ctx, g20p);
		auto g21 = m::gap(ctx, g21p);
		auto c2 = m::container(ctx, c2p, ruis::widget_list{g20, g21});
		auto c = m::container(ctx, ruis::container::all_parameters{}, ruis::widget_list{c2});

		// run 'get_state' on the widget with id 'g21' reached through 'c2':
		// c2 is looked up on c, then g21 is looked up on c2
		const auto resp = c.get().probe(make_for_id_cmd({"c2", "g21"}, "get_state"));
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

	suite.add("for_id_empty", [] {
		auto ctx = make_dummy_context();

		auto c = m::container(ctx, ruis::container::all_parameters{}, ruis::widget_list{});

		// an empty id path runs the command on this container itself
		auto resp = c.get().probe(make_for_id_cmd_empty("get_children"));
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

	suite.add("for_id_not_found", [] {
		auto ctx = make_dummy_context();

		// c
		//   [0] container id="L"  [0] gap id="a"
		//   [1] container id="R"  [0] gap id="b"
		ruis::gap::all_parameters ap;
		ap.widget.id = "a";
		ruis::gap::all_parameters bp;
		bp.widget.id = "b";
		ruis::container::all_parameters lp;
		lp.widget.id = "L";
		ruis::container::all_parameters rp;
		rp.widget.id = "R";
		auto a = m::gap(ctx, ap);
		auto b = m::gap(ctx, bp);
		auto L = m::container(ctx, lp, ruis::widget_list{a});
		auto R = m::container(ctx, rp, ruis::widget_list{b});
		auto c = m::container(ctx, ruis::container::all_parameters{}, ruis::widget_list{L, R});

		// the 1st id does not exist anywhere in the tree:
		// error { id { nope } not_found }
		{
			auto resp = c.get().probe(make_for_id_cmd({"nope"}, "get_state"));
			const tml::forest f(resp.begin(), resp.end());
			tst::check_eq(f.size(), std::size_t(1), SL);
			const auto& err = tml::const_crawler(f).to("ruis::container").in().to("error").get();
			tst::check_eq(err.children.size(), std::size_t(2), SL);
			tst::check(
				err.children[0].value == "id",
				[&](auto& o) {
					o << err.children[0].value.string;
				},
				SL
			);
			tst::check_eq(err.children[0].children.size(), std::size_t(1), SL);
			tst::check(
				err.children[0].children[0].value == "nope",
				[&](auto& o) {
					o << err.children[0].children[0].value.string;
				},
				SL
			);
			tst::check(
				err.children[1].value == "not_found",
				[&](auto& o) {
					o << err.children[1].value.string;
				},
				SL
			);
		}

		// 'b' exists in the tree but not under 'L', so the 2nd step fails:
		// error { id { L b } not_found }
		{
			auto resp = c.get().probe(make_for_id_cmd({"L", "b"}, "get_state"));
			const tml::forest f(resp.begin(), resp.end());
			tst::check_eq(f.size(), std::size_t(1), SL);
			const auto& err = tml::const_crawler(f).to("ruis::container").in().to("error").get();
			tst::check_eq(err.children.size(), std::size_t(2), SL);
			tst::check_eq(err.children[0].children.size(), std::size_t(2), SL);
			tst::check(
				err.children[0].children[0].value == "L",
				[&](auto& o) {
					o << err.children[0].children[0].value.string;
				},
				SL
			);
			tst::check(
				err.children[0].children[1].value == "b",
				[&](auto& o) {
					o << err.children[0].children[1].value.string;
				},
				SL
			);
			tst::check(
				err.children[1].value == "not_found",
				[&](auto& o) {
					o << err.children[1].value.string;
				},
				SL
			);
		}
	});
});
} // namespace
