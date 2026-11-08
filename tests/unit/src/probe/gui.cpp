#include <cstddef>
#include <memory>

#include <ruis/gui.hpp>
#include <ruis/util/events.hpp>
#include <ruis/widget/container.hpp>
#include <ruis/widget/input/base/character_input_widget.hpp>
#include <tml/crawler.hpp>
#include <tml/tree.hpp>
#include <tst/check.hpp>
#include <tst/set.hpp>

#include "../../../harness/util/dummy_context.hpp"

namespace {

// a character input widget which records the last received character input string
class char_input_recorder : public ruis::character_input_widget
{
	std::u32string received;

public:
	explicit char_input_recorder(const utki::shared_ref<ruis::context>& ctx) :
		widget(ctx, ruis::layout_parameters{}, ruis::widget::parameters{}),
		character_input_widget(ctx)
	{}

	void on_character_input(const ruis::character_input_event& e) override
	{
		this->received = e.string;
	}

	const std::u32string& get_received() const noexcept
	{
		return this->received;
	}
};

// make a gui with a 100x100 viewport and a container root widget
std::unique_ptr<ruis::gui> make_gui(const utki::shared_ref<ruis::context>& ctx, utki::shared_ref<ruis::container> root)
{
	auto gui = std::make_unique<ruis::gui>(ctx);
	gui->set_viewport(ruis::rect(0, 0, 100, 100));
	gui->set_root(root);
	return gui;
}

// check that the response is a single 'ruis::gui' section with an 'ok' node
void check_ok_response(const tml::forest& f)
{
	tst::check(
		f.size() == 1,
		[&](auto& o) {
			o << f.size();
		},
		SL
	);
	tst::check(
		f[0].value == "ruis::gui",
		[&](auto& o) {
			o << f[0].value.string;
		},
		SL
	);
	const auto& ok = tml::const_crawler(f) //
						 .to("ruis::gui") //
						 .in() //
						 .to("ok") //
						 .get();
	tst::check(ok.children.empty(), SL);
}

// check that the response is a single 'ruis::gui' section with an 'error { <diagnostic> }' node
void check_error_response(const tml::forest& f, const char* diagnostic)
{
	tst::check(
		f.size() == 1,
		[&](auto& o) {
			o << f.size();
		},
		SL
	);
	tst::check(
		f[0].value == "ruis::gui",
		[&](auto& o) {
			o << f[0].value.string;
		},
		SL
	);
	const auto& err = tml::const_crawler(f) //
						  .to("ruis::gui") //
						  .in() //
						  .to("error") //
						  .in() //
						  .get()
						  .value;
	tst::check(
		err == diagnostic,
		[&](auto& o) {
			o << err.string;
		},
		SL
	);
}

} // namespace

namespace {

const tst::set set("gui_probe", [](tst::suite& suite) {
	// Note: the probe response structure is deterministic, so the tests
	// navigate it with tml::const_crawler cascades - a structural mismatch
	// (regression) fails the test with an exception thrown by the crawler.

	suite.add("unknown_command_returns_empty", [] {
		auto ctx = make_dummy_context();
		auto root = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list{});
		auto gui = make_gui(ctx, root);

		const auto resp = gui->probe(tml::tree(tml::leaf("bogus_command")));
		tst::check_eq(resp.size(), std::size_t(0), SL);
	});

	suite.add("send_mouse_move_ok", [] {
		auto ctx = make_dummy_context();
		auto root = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list{});
		auto gui = make_gui(ctx, root);

		// move the mouse pointer inside the root widget's rect
		const auto resp = gui->probe(tml::tree(
			tml::leaf("send_mouse_move"),
			{tml::tree(tml::leaf("pos"), tml::forest{tml::tree(tml::leaf(1)), tml::tree(tml::leaf(1))}),
			 tml::tree(tml::leaf("id"), tml::forest{tml::tree(tml::leaf(0))})}
		));
		check_ok_response(resp);
		tst::check(root.get().is_hovered(0), SL);
	});

	suite.add("send_mouse_move_missing_pos", [] {
		auto ctx = make_dummy_context();
		auto root = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list{});
		auto gui = make_gui(ctx, root);

		const auto resp = gui->probe(tml::tree(
			tml::leaf("send_mouse_move"), //
			{tml::tree(tml::leaf("id"), tml::forest{tml::tree(tml::leaf(0))})}
		));
		check_error_response(resp, "crawler::to() failed, reached end of node list");
	});

	suite.add("send_mouse_move_pos_is_filled", [] {
		auto ctx = make_dummy_context();
		auto root = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list{});
		auto gui = make_gui(ctx, root);

		// a position with a single component is filled with the parsed value (make_vec2 semantics)
		const auto resp = gui->probe(tml::tree(
			tml::leaf("send_mouse_move"), //
			{tml::tree(tml::leaf("pos"), tml::forest{tml::tree(tml::leaf(5))}),
			 tml::tree(tml::leaf("id"), tml::forest{tml::tree(tml::leaf(0))})}
		));
		check_ok_response(resp);
		tst::check(root.get().is_hovered(0), SL);
	});

	suite.add("send_mouse_move_malformed_pos", [] {
		auto ctx = make_dummy_context();
		auto root = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list{});
		auto gui = make_gui(ctx, root);

		// a non-numeric component is malformed; the diagnostic is
		// the message of the exception thrown by the tml leaf numeric conversion
		const auto resp = gui->probe(tml::tree(
			tml::leaf("send_mouse_move"), //
			{tml::tree(tml::leaf("pos"), tml::forest{tml::tree(tml::leaf(1)), tml::tree(tml::leaf("abc"))}),
			 tml::tree(tml::leaf("id"), tml::forest{tml::tree(tml::leaf(0))})}
		));
		check_error_response(resp, "string_parser::read_integer(): input string does not start with a number");
	});

	suite.add("send_mouse_move_missing_id", [] {
		auto ctx = make_dummy_context();
		auto root = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list{});
		auto gui = make_gui(ctx, root);

		const auto resp = gui->probe(tml::tree(
			tml::leaf("send_mouse_move"), //
			{tml::tree(tml::leaf("pos"), tml::forest{tml::tree(tml::leaf(1)), tml::tree(tml::leaf(1))})}
		));
		check_error_response(resp, "crawler::to() failed, reached end of node list");
	});

	suite.add("send_mouse_button_ok", [] {
		auto ctx = make_dummy_context();
		auto root = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list{});
		auto gui = make_gui(ctx, root);

		const auto resp = gui->probe(tml::tree(
			tml::leaf("send_mouse_button"),
			{tml::tree(tml::leaf("action"), tml::forest{tml::tree(tml::leaf("press"))}),
			 tml::tree(tml::leaf("pos"), tml::forest{tml::tree(tml::leaf(1)), tml::tree(tml::leaf(1))}),
			 tml::tree(tml::leaf("button"), tml::forest{tml::tree(tml::leaf("left"))}),
			 tml::tree(tml::leaf("id"), tml::forest{tml::tree(tml::leaf(0))})}
		));
		check_ok_response(resp);
		tst::check(root.get().is_hovered(0), SL);
	});

	suite.add("send_mouse_button_malformed_button", [] {
		auto ctx = make_dummy_context();
		auto root = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list{});
		auto gui = make_gui(ctx, root);

		const auto resp = gui->probe(tml::tree(
			tml::leaf("send_mouse_button"),
			{tml::tree(tml::leaf("action"), tml::forest{tml::tree(tml::leaf("press"))}),
			 tml::tree(tml::leaf("pos"), tml::forest{tml::tree(tml::leaf(1)), tml::tree(tml::leaf(1))}),
			 tml::tree(tml::leaf("button"), tml::forest{tml::tree(tml::leaf("diagonal"))}),
			 tml::tree(tml::leaf("id"), tml::forest{tml::tree(tml::leaf(0))})}
		));
		check_error_response(resp, "malformed_button");
	});

	suite.add("send_mouse_button_malformed_action", [] {
		auto ctx = make_dummy_context();
		auto root = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list{});
		auto gui = make_gui(ctx, root);

		const auto resp = gui->probe(tml::tree(
			tml::leaf("send_mouse_button"),
			{tml::tree(tml::leaf("action"), tml::forest{tml::tree(tml::leaf("hold"))}),
			 tml::tree(tml::leaf("pos"), tml::forest{tml::tree(tml::leaf(1)), tml::tree(tml::leaf(1))}),
			 tml::tree(tml::leaf("button"), tml::forest{tml::tree(tml::leaf("left"))}),
			 tml::tree(tml::leaf("id"), tml::forest{tml::tree(tml::leaf(0))})}
		));
		check_error_response(resp, "malformed_action");
	});

	suite.add("send_key_ok", [] {
		auto ctx = make_dummy_context();
		auto root = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list{});
		auto gui = make_gui(ctx, root);

		// there is no focused widget, so the event is routed to the default key handler
		ruis::key_event e;
		gui->default_key_handler = [&](ruis::key_event ev) {
			e = ev;
		};

		const auto resp = gui->probe(tml::tree(
			tml::leaf("send_key"),
			{tml::tree(tml::leaf("action"), tml::forest{tml::tree(tml::leaf("press"))}),
			 tml::tree(tml::leaf("key"), tml::forest{tml::tree(tml::leaf("enter"))})}
		));
		check_ok_response(resp);
		tst::check(
			e.action == ruis::button_action::press,
			[&](auto& o) {
				o << static_cast<int>(e.action);
			},
			SL
		);
		tst::check(
			e.combo.key == ruis::key::enter,
			[&](auto& o) {
				o << static_cast<int>(e.combo.key);
			},
			SL
		);
	});

	suite.add("send_key_missing_key", [] {
		auto ctx = make_dummy_context();
		auto root = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list{});
		auto gui = make_gui(ctx, root);

		const auto resp = gui->probe(tml::tree(
			tml::leaf("send_key"), //
			{tml::tree(tml::leaf("action"), tml::forest{tml::tree(tml::leaf("press"))})}
		));
		check_error_response(resp, "crawler::to() failed, reached end of node list");
	});

	suite.add("send_key_unknown_key", [] {
		auto ctx = make_dummy_context();
		auto root = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list{});
		auto gui = make_gui(ctx, root);

		const auto resp = gui->probe(tml::tree(
			tml::leaf("send_key"),
			{tml::tree(tml::leaf("action"), tml::forest{tml::tree(tml::leaf("press"))}),
			 tml::tree(tml::leaf("key"), tml::forest{tml::tree(tml::leaf("hyper"))})}
		));
		check_error_response(resp, "malformed_key");
	});

	suite.add("send_character_input_ok", [] {
		auto ctx = make_dummy_context();
		auto root = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list{});
		auto gui = make_gui(ctx, root);

		// focus a character input widget and feed it some text
		auto rec = utki::make_shared<char_input_recorder>(ctx);
		rec.get().focus();

		const auto resp = gui->probe(tml::tree(
			tml::leaf("send_character_input"),
			{tml::tree(tml::leaf("string"), tml::forest{tml::tree(tml::leaf("abc"))}),
			 tml::tree(tml::leaf("key"), tml::forest{tml::tree(tml::leaf("c"))})}
		));
		check_ok_response(resp);
		tst::check(
			rec.get().get_received() == std::u32string{u'a', u'b', u'c'},
			[&](auto& o) {
				o << rec.get().get_received().size();
			},
			SL
		);
	});

	suite.add("send_character_input_missing_string", [] {
		auto ctx = make_dummy_context();
		auto root = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list{});
		auto gui = make_gui(ctx, root);

		const auto resp = gui->probe(tml::tree(
			tml::leaf("send_character_input"), //
			{tml::tree(tml::leaf("key"), tml::forest{tml::tree(tml::leaf("c"))})}
		));
		check_error_response(resp, "crawler::to() failed, reached end of node list");
	});

	suite.add("for_root", [] {
		auto ctx = make_dummy_context();
		auto root = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list{});
		auto gui = make_gui(ctx, root);

		// the response must be exactly the flattened root widget's probe response
		const auto resp = gui->probe(tml::tree(
			tml::leaf("for_root"), //
			{tml::tree(tml::leaf("get_state"))}
		));
		const auto direct = root.get().probe(tml::tree(tml::leaf("get_state")));
		const tml::forest direct_f(direct.begin(), direct.end());
		tst::check(resp == direct_f, SL);
	});

	suite.add("for_focused_no_focus", [] {
		auto ctx = make_dummy_context();
		auto root = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list{});
		auto gui = make_gui(ctx, root);

		const auto resp = gui->probe(tml::tree(
			tml::leaf("for_focused"), //
			{tml::tree(tml::leaf("get_state"))}
		));
		check_error_response(resp, "no_focused_widget");
	});

	suite.add("for_focused", [] {
		auto ctx = make_dummy_context();
		auto root = utki::make_shared<ruis::container>(ctx, ruis::container::all_parameters(), ruis::widget_list{});
		auto gui = make_gui(ctx, root);

		// focus the root widget; the response must be exactly its flattened probe response
		root.get().focus();
		const auto resp = gui->probe(tml::tree(
			tml::leaf("for_focused"), //
			{tml::tree(tml::leaf("get_state"))}
		));
		const auto direct = root.get().probe(tml::tree(tml::leaf("get_state")));
		const tml::forest direct_f(direct.begin(), direct.end());
		tst::check(resp == direct_f, SL);
	});
});
} // namespace
