#include <tst/set.hpp>
#include <tst/check.hpp>

#include <ruis/util/color.hpp>

#include <tml/tree.hpp>

namespace {
const tst::set set("color", [](tst::suite& suite) {

	suite.add("default_constructor_creates_undefined_color", [] {
		ruis::color c;
		tst::check(c.is_undefined(), SL);
	});

	suite.add("uint32_constructor_parses_rgba", [] {
		ruis::color c(0xFF804010);
		tst::check_eq(c.r(), uint8_t(0x10), SL);
		tst::check_eq(c.g(), uint8_t(0x40), SL);
		tst::check_eq(c.b(), uint8_t(0x80), SL);
		tst::check_eq(c.a(), uint8_t(0xFF), SL);
	});

	suite.add("four_channel_constructor", [] {
		ruis::color c(10, 20, 30, 255);
		tst::check_eq(c.r(), uint8_t(10), SL);
		tst::check_eq(c.g(), uint8_t(20), SL);
		tst::check_eq(c.b(), uint8_t(30), SL);
		tst::check_eq(c.a(), uint8_t(255), SL);
	});

	suite.add("to_uint32_t_roundtrip", [] {
		uint32_t original = 0xAABBCCDD;
		ruis::color c(original);
		tst::check_eq(c.to_uint32_t(), original, SL);
	});

	suite.add("to_vec4f_normalizes_to_unit_range", [] {
		ruis::color c(255, 128, 0, 255);
		auto v = c.to_vec4f();
		tst::check_eq(v.x(), 1.0f, SL);
		tst::check_eq(v.y(), 128.0f / 255.0f, SL);
		tst::check_eq(v.z(), 0.0f, SL);
		tst::check_eq(v.w(), 1.0f, SL);
	});

	suite.add("to_vec4f_zero_color", [] {
		ruis::color c(0, 0, 0, 0);
		auto v = c.to_vec4f();
		tst::check_eq(v.x(), 0.0f, SL);
		tst::check_eq(v.y(), 0.0f, SL);
		tst::check_eq(v.z(), 0.0f, SL);
		tst::check_eq(v.w(), 0.0f, SL);
	});

	suite.add("is_undefined_true_for_zero", [] {
		ruis::color c(0);
		tst::check(c.is_undefined(), SL);
	});

	suite.add("is_undefined_false_for_nonzero", [] {
		ruis::color c(0xFF000001);
		tst::check(!c.is_undefined(), SL);
	});

	suite.add("is_undefined_false_for_fully_transparent_but_visible", [] {
		// fully transparent but not undefined
		ruis::color c(0x000000FF);
		tst::check(!c.is_undefined(), SL);
	});

	suite.add("equality", [] {
		ruis::color a(0xFF112233);
		ruis::color b(0xFF112233);
		ruis::color c(0xFF445566);
		tst::check(a == b, SL);
		tst::check(a != c, SL);
	});

	suite.add("index_access_matches_named_access", [] {
		ruis::color c(1, 2, 3, 4);
		tst::check_eq(c[0], c.r(), SL);
		tst::check_eq(c[1], c.g(), SL);
		tst::check_eq(c[2], c.b(), SL);
		tst::check_eq(c[3], c.a(), SL);
	});


	suite.add("make_from_empty_forest_returns_undefined", [] {
		tml::forest f;
		auto c = ruis::color::make_from(f);
		tst::check(c.is_undefined(), SL);
	});

	suite.add("make_from_forest_with_value", [] {
		tml::forest f;
		f.emplace_back(tml::tree(tml::leaf("0xff804010")));

		auto c = ruis::color::make_from(f);
		tst::check_eq(c.r(), uint8_t(0x10), SL);
		tst::check_eq(c.g(), uint8_t(0x40), SL);
		tst::check_eq(c.b(), uint8_t(0x80), SL);
		tst::check_eq(c.a(), uint8_t(0xFF), SL);
	});

	suite.add("fully_opaque_white", [] {
		ruis::color c(0xFFFFFFFF);
		tst::check_eq(c.r(), uint8_t(255), SL);
		tst::check_eq(c.g(), uint8_t(255), SL);
		tst::check_eq(c.b(), uint8_t(255), SL);
		tst::check_eq(c.a(), uint8_t(255), SL);
		tst::check(!c.is_undefined(), SL);
	});

	suite.add("fully_opaque_black", [] {
		ruis::color c(0xFF000000);
		tst::check_eq(c.r(), uint8_t(0), SL);
		tst::check_eq(c.g(), uint8_t(0), SL);
		tst::check_eq(c.b(), uint8_t(0), SL);
		tst::check_eq(c.a(), uint8_t(255), SL);
		tst::check(!c.is_undefined(), SL);
	});

});
}
