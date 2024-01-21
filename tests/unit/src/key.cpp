#include <tst/set.hpp>
#include <tst/check.hpp>

#include <ruis/util/key.hpp>
#include <ruis/util/events.hpp>

using namespace std::string_view_literals;
using namespace std::string_literals;

namespace{
const std::vector<std::pair<ruis::key, std::string_view>>
expected_key_to_name_map = {
	{ruis::key::space, "space"sv},
	{ruis::key::enter, "enter"sv},
	{ruis::key::zero, "0"sv},
	{ruis::key::one, "1"sv},
	{ruis::key::two, "2"sv},
	{ruis::key::three, "3"sv},
	{ruis::key::four, "4"sv},
	{ruis::key::five, "5"sv},
	{ruis::key::six, "6"sv},
	{ruis::key::seven, "7"sv},
	{ruis::key::eight, "8"sv},
	{ruis::key::nine, "9"sv},
	{ruis::key::a, "a"sv},
	{ruis::key::b, "b"sv},
	{ruis::key::c, "c"sv},
	{ruis::key::d, "d"sv},
	{ruis::key::e, "e"sv},
	{ruis::key::f, "f"sv},
	{ruis::key::g, "g"sv},
	{ruis::key::h, "h"sv},
	{ruis::key::i, "i"sv},
	{ruis::key::j, "j"sv},
	{ruis::key::k, "k"sv},
	{ruis::key::l, "l"sv},
	{ruis::key::m, "m"sv},
	{ruis::key::n, "n"sv},
	{ruis::key::o, "o"sv},
	{ruis::key::p, "p"sv},
	{ruis::key::q, "q"sv},
	{ruis::key::r, "r"sv},
	{ruis::key::s, "s"sv},
	{ruis::key::t, "t"sv},
	{ruis::key::u, "u"sv},
	{ruis::key::v, "v"sv},
	{ruis::key::w, "w"sv},
	{ruis::key::x, "x"sv},
	{ruis::key::y, "y"sv},
	{ruis::key::z, "z"sv},
	{ruis::key::arrow_left, "arrow_left"sv},
	{ruis::key::arrow_right, "arrow_right"sv},
	{ruis::key::arrow_up, "arrow_up"sv},
	{ruis::key::arrow_down, "arrow_down"sv},
	{ruis::key::comma, "comma"sv},
	{ruis::key::semicolon, "semicolon"sv},
	{ruis::key::apostrophe, "apostrophe"sv},
	{ruis::key::period, "period"sv},
	{ruis::key::slash, "slash"sv},
	{ruis::key::backslash, "backslash"sv},
	{ruis::key::tabulator, "tabulator"sv},
	{ruis::key::left_shift, "left_shift"sv},
	{ruis::key::right_shift, "right_shift"sv},
	{ruis::key::end, "end"sv},
	{ruis::key::left_square_bracket, "left_square_bracket"sv},
	{ruis::key::right_square_bracket, "right_square_bracket"sv},
	{ruis::key::grave, "grave"sv},
	{ruis::key::minus, "minus"sv},
	{ruis::key::equals, "equals"sv},
	{ruis::key::backspace, "backspace"sv},
	{ruis::key::capslock, "capslock"sv},
	{ruis::key::escape, "escape"sv},
	{ruis::key::left_control, "left_control"sv},
	{ruis::key::left_alt, "left_alt"sv},
	{ruis::key::f1, "f1"sv},
	{ruis::key::f2, "f2"sv},
	{ruis::key::f3, "f3"sv},
	{ruis::key::f4, "f4"sv},
	{ruis::key::f5, "f5"sv},
	{ruis::key::f6, "f6"sv},
	{ruis::key::f7, "f7"sv},
	{ruis::key::f8, "f8"sv},
	{ruis::key::f9, "f9"sv},
	{ruis::key::f10, "f10"sv},
	{ruis::key::f11, "f11"sv},
	{ruis::key::f12, "f12"sv},
	{ruis::key::right_control, "right_control"sv},
	{ruis::key::print_screen, "print_screen"sv},
	{ruis::key::right_alt, "right_alt"sv},
	{ruis::key::home, "home"sv},
	{ruis::key::page_up, "page_up"sv},
	{ruis::key::page_down, "page_down"sv},
	{ruis::key::insert, "insert"sv},
	{ruis::key::deletion, "deletion"sv},
	{ruis::key::pause, "pause"sv},
	{ruis::key::left_command, "left_command"sv},
	{ruis::key::right_command, "right_command"sv},
	{ruis::key::menu, "menu"sv},
	{ruis::key::function, "function"sv},
	{ruis::key::f17, "f17"sv},
	{ruis::key::f18, "f18"sv},
	{ruis::key::f19, "f19"sv},
	{ruis::key::f20, "f20"sv},
	{ruis::key::f13, "f13"sv},
	{ruis::key::f16, "f16"sv},
	{ruis::key::f14, "f14"sv},
	{ruis::key::f15, "f15"sv},
};
}

namespace{
const tst::set set("key", [](tst::suite& suite){
	suite.add("morda_key_enum_size_is_as_expected", []{
		tst::check_eq(expected_key_to_name_map.size(), size_t(ruis::key::enum_size), SL);
	});

	suite.add<std::pair<ruis::key, std::string_view>>(
			"key_to_string",
			decltype(expected_key_to_name_map)(expected_key_to_name_map),
			[](const auto& p){
				tst::check_eq(ruis::to_string(p.first), p.second, SL);
			}
		);
	
	suite.add<std::pair<ruis::key, std::string_view>>(
			"string_to_key",
			decltype(expected_key_to_name_map)(expected_key_to_name_map),
			[](const auto& p){
				auto key = ruis::to_key(p.second);
				tst::check(key == p.first, SL) << "expected " << size_t(p.first) << ", got " << size_t(key);
			}
	);

	suite.add("invalid_key_name", []{
		auto key = ruis::to_key("arrr");
		tst::check(key == ruis::key::unknown, SL) << "key = " << ruis::to_string(key);
	});

	suite.add("unknown_key_to_string", []{
		tst::check_eq(
				std::string(ruis::to_string(ruis::key::unknown)),
				"unknown"s,
				SL
			);
	});

	suite.add("key_combo_can_be_compared_for_equality__equal_combos", []{
		ruis::key_combo c1;
		c1.key = ruis::key::enter;
		c1.modifiers = ruis::key_modifier::left_alt;

		ruis::key_combo c2;
		c2.key = ruis::key::enter;
		c2.modifiers = ruis::key_modifier::left_alt;

		tst::check(c1 == c2, SL);
	});

	suite.add("key_combo_can_be_compared_for_equality__different_keys", []{
		ruis::key_combo c1;
		c1.key = ruis::key::space;
		c1.modifiers = ruis::key_modifier::left_alt;

		ruis::key_combo c2;
		c2.key = ruis::key::enter;
		c2.modifiers = ruis::key_modifier::left_alt;

		tst::check(!(c1 == c2), SL);
	});

	suite.add("key_combo_can_be_compared_for_equality__different_modifiers", []{
		ruis::key_combo c1;
		c1.key = ruis::key::enter;
		c1.modifiers = ruis::key_modifier::right_alt;

		ruis::key_combo c2;
		c2.key = ruis::key::enter;
		c2.modifiers = ruis::key_modifier::left_alt;

		tst::check(!(c1 == c2), SL);
	});

	suite.add("key_combo_can_be_compared_for_equality__different_combos", []{
		ruis::key_combo c1;
		c1.key = ruis::key::arrow_left;
		c1.modifiers = ruis::key_modifier::right_alt;

		ruis::key_combo c2;
		c2.key = ruis::key::enter;
		c2.modifiers = ruis::key_modifier::left_alt;

		tst::check(!(c1 == c2), SL);
	});

	suite.add("key_combo_can_be_compared_for_inequality__different_modifiers", []{
		ruis::key_combo c1;
		c1.key = ruis::key::enter;
		c1.modifiers = ruis::key_modifier::right_alt;

		ruis::key_combo c2;
		c2.key = ruis::key::enter;
		c2.modifiers = ruis::key_modifier::left_alt;

		tst::check(c1 != c2, SL);
	});

	suite.add("key_combo_can_be_compared_for_inequality__different_keys", []{
		ruis::key_combo c1;
		c1.key = ruis::key::arrow_left;
		c1.modifiers = ruis::key_modifier::left_alt;
		
		ruis::key_combo c2;
		c2.key = ruis::key::enter;
		c2.modifiers = ruis::key_modifier::left_alt;

		tst::check(c1 != c2, SL);
	});

	suite.add("key_combo_can_be_compared_for_inequality__different_combos", []{
		ruis::key_combo c1;
		c1.key = ruis::key::arrow_left;
		c1.modifiers = ruis::key_modifier::right_alt | ruis::key_modifier::left_command;

		ruis::key_combo c2;
		c2.key = ruis::key::enter;
		c2.modifiers = ruis::key_modifier::left_alt;

		tst::check(c1 != c2, SL);
	});

	suite.add("key_combo_can_be_compared_for_inequality__equal_combos", []{
		ruis::key_combo c1;
		c1.key = ruis::key::enter;
		c1.modifiers = ruis::key_modifier::right_alt | ruis::key_modifier::left_command;

		ruis::key_combo c2;
		c2.key = ruis::key::enter;
		c2.modifiers = ruis::key_modifier::right_alt | ruis::key_modifier::left_command;

		tst::check(!(c1 != c2), SL);
	});
});
}
