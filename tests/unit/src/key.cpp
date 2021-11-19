#include <tst/set.hpp>
#include <tst/check.hpp>

#include <morda/util/key.hpp>

using namespace std::string_view_literals;

namespace{
std::vector<std::pair<morda::key, std::string_view>>
expected_key_to_name_map = {
	{morda::key::space, "space"sv},
	{morda::key::enter, "enter"sv},
	{morda::key::zero, "0"sv},
	{morda::key::one, "1"sv},
	{morda::key::two, "2"sv},
	{morda::key::three, "3"sv},
	{morda::key::four, "4"sv},
	{morda::key::five, "5"sv},
	{morda::key::six, "6"sv},
	{morda::key::seven, "7"sv},
	{morda::key::eight, "8"sv},
	{morda::key::nine, "9"sv},
	{morda::key::a, "a"sv},
	{morda::key::b, "b"sv},
	{morda::key::c, "c"sv},
	{morda::key::d, "d"sv},
	{morda::key::e, "e"sv},
	{morda::key::f, "f"sv},
	{morda::key::g, "g"sv},
	{morda::key::h, "h"sv},
	{morda::key::i, "i"sv},
	{morda::key::j, "j"sv},
	{morda::key::k, "k"sv},
	{morda::key::l, "l"sv},
	{morda::key::m, "m"sv},
	{morda::key::n, "n"sv},
	{morda::key::o, "o"sv},
	{morda::key::p, "p"sv},
	{morda::key::q, "q"sv},
	{morda::key::r, "r"sv},
	{morda::key::s, "s"sv},
	{morda::key::t, "t"sv},
	{morda::key::u, "u"sv},
	{morda::key::v, "v"sv},
	{morda::key::w, "w"sv},
	{morda::key::x, "x"sv},
	{morda::key::y, "y"sv},
	{morda::key::z, "z"sv},
	{morda::key::arrow_left, "arrow_left"sv},
	{morda::key::arrow_right, "arrow_right"sv},
	{morda::key::arrow_up, "arrow_up"sv},
	{morda::key::arrow_down, "arrow_down"sv},
	{morda::key::comma, "comma"sv},
	{morda::key::semicolon, "semicolon"sv},
	{morda::key::apostrophe, "apostrophe"sv},
	{morda::key::period, "period"sv},
	{morda::key::slash, "slash"sv},
	{morda::key::backslash, "backslash"sv},
	{morda::key::tabulator, "tabulator"sv},
	{morda::key::left_shift, "left_shift"sv},
	{morda::key::right_shift, "right_shift"sv},
	{morda::key::end, "end"sv},
	{morda::key::left_square_bracket, "left_square_bracket"sv},
	{morda::key::right_square_bracket, "right_square_bracket"sv},
	{morda::key::grave, "grave"sv},
	{morda::key::minus, "minus"sv},
	{morda::key::equals, "equals"sv},
	{morda::key::backspace, "backspace"sv},
	{morda::key::capslock, "capslock"sv},
	{morda::key::escape, "escape"sv},
	{morda::key::left_control, "left_control"sv},
	{morda::key::left_alt, "left_alt"sv},
	{morda::key::f1, "f1"sv},
	{morda::key::f2, "f2"sv},
	{morda::key::f3, "f3"sv},
	{morda::key::f4, "f4"sv},
	{morda::key::f5, "f5"sv},
	{morda::key::f6, "f6"sv},
	{morda::key::f7, "f7"sv},
	{morda::key::f8, "f8"sv},
	{morda::key::f9, "f9"sv},
	{morda::key::f10, "f10"sv},
	{morda::key::f11, "f11"sv},
	{morda::key::f12, "f12"sv},
	{morda::key::right_control, "right_control"sv},
	{morda::key::print_screen, "print_screen"sv},
	{morda::key::right_alt, "right_alt"sv},
	{morda::key::home, "home"sv},
	{morda::key::page_up, "page_up"sv},
	{morda::key::page_down, "page_down"sv},
	{morda::key::insert, "insert"sv},
	{morda::key::deletion, "deletion"sv},
	{morda::key::pause, "pause"sv},
	{morda::key::left_command, "left_command"sv},
	{morda::key::right_command, "right_command"sv},
	{morda::key::menu, "menu"sv},
	{morda::key::function, "function"sv},
	{morda::key::f17, "f17"sv},
	{morda::key::f18, "f18"sv},
	{morda::key::f19, "f19"sv},
	{morda::key::f20, "f20"sv},
	{morda::key::f13, "f13"sv},
	{morda::key::f16, "f16"sv},
	{morda::key::f14, "f14"sv},
	{morda::key::f15, "f15"sv},
};
}

namespace{
tst::set set("key", [](tst::suite& suite){
	suite.add("morda_key_enum_size_is_as_expected", []{
		tst::check_eq(expected_key_to_name_map.size(), size_t(morda::key::enum_size), SL);
	});

	suite.add<std::pair<morda::key, std::string_view>>(
			"key_to_string",
			decltype(expected_key_to_name_map)(expected_key_to_name_map),
			[](const auto& p){
				tst::check_eq(morda::to_string(p.first), p.second, SL);
			}
		);
	
	suite.add<std::pair<morda::key, std::string_view>>(
			"string_to_key",
			decltype(expected_key_to_name_map)(expected_key_to_name_map),
			[](const auto& p){
				auto key = morda::to_key(p.second);
				tst::check(key == p.first, SL) << "expected " << size_t(p.first) << ", got " << size_t(key);
			}
	);
});
}
