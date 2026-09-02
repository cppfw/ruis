#include <tst/set.hpp>
#include <tst/check.hpp>

#include <ruis/util/corners.hpp>

namespace {
const tst::set set("corners", [](tst::suite& suite) {

	suite.add("default_constructor_initializes_to_zero", [] {
		ruis::corners<int> c;
		tst::check(c.is_zero(), SL);
		tst::check_eq(c[0], 0, SL);
		tst::check_eq(c[1], 0, SL);
		tst::check_eq(c[2], 0, SL);
		tst::check_eq(c[3], 0, SL);
	});

	suite.add("single_value_constructor_sets_all_corners", [] {
		ruis::corners<int> c(5);
		tst::check_eq(c.left_top(), 5, SL);
		tst::check_eq(c.right_top(), 5, SL);
		tst::check_eq(c.right_bottom(), 5, SL);
		tst::check_eq(c.left_bottom(), 5, SL);
	});

	suite.add("four_value_constructor_sets_each_corner", [] {
		ruis::corners<int> c(1, 2, 3, 4);
		tst::check_eq(c.left_top(), 1, SL);
		tst::check_eq(c.right_top(), 2, SL);
		tst::check_eq(c.right_bottom(), 3, SL);
		tst::check_eq(c.left_bottom(), 4, SL);
	});

	suite.add("index_access_matches_named_access", [] {
		ruis::corners<int> c(10, 20, 30, 40);
		tst::check_eq(c[0], c.left_top(), SL);
		tst::check_eq(c[1], c.right_top(), SL);
		tst::check_eq(c[2], c.right_bottom(), SL);
		tst::check_eq(c[3], c.left_bottom(), SL);
	});

	suite.add("mutable_named_accessors_allow_modification", [] {
		ruis::corners<int> c(0);
		c.left_top() = 1;
		c.right_top() = 2;
		c.right_bottom() = 3;
		c.left_bottom() = 4;
		tst::check_eq(c.left_top(), 1, SL);
		tst::check_eq(c.right_top(), 2, SL);
		tst::check_eq(c.right_bottom(), 3, SL);
		tst::check_eq(c.left_bottom(), 4, SL);
	});

	suite.add("top_is_sum_of_left_top_and_right_top", [] {
		ruis::corners<int> c(1, 2, 3, 4);
		tst::check_eq(c.top(), 3, SL);
	});

	suite.add("bottom_is_sum_of_left_bottom_and_right_bottom", [] {
		ruis::corners<int> c(1, 2, 3, 4);
		tst::check_eq(c.bottom(), 7, SL);
	});

	suite.add("left_is_sum_of_left_top_and_left_bottom", [] {
		ruis::corners<int> c(1, 2, 3, 4);
		tst::check_eq(c.left(), 5, SL);
	});

	suite.add("right_is_sum_of_right_top_and_right_bottom", [] {
		ruis::corners<int> c(1, 2, 3, 4);
		tst::check_eq(c.right(), 5, SL);
	});

	suite.add("is_zero_returns_true_for_all_zeros", [] {
		ruis::corners<int> c(0);
		tst::check(c.is_zero(), SL);
	});

	suite.add("is_zero_returns_false_when_any_corner_nonzero", [] {
		ruis::corners<int> c(0, 0, 0, 1);
		tst::check(!c.is_zero(), SL);
	});

	suite.add("operator_assign_sets_all_corners", [] {
		ruis::corners<int> c(1, 2, 3, 4);
		c = 7;
		tst::check_eq(c.left_top(), 7, SL);
		tst::check_eq(c.right_top(), 7, SL);
		tst::check_eq(c.right_bottom(), 7, SL);
		tst::check_eq(c.left_bottom(), 7, SL);
	});

	suite.add("equality_via_array_inheritance", [] {
		ruis::corners<int> a(1, 2, 3, 4);
		ruis::corners<int> b(1, 2, 3, 4);
		ruis::corners<int> c(1, 2, 3, 5);
		tst::check(a == b, SL);
		tst::check(a != c, SL);
	});

	suite.add("operator_less__equal", [] {
		ruis::corners<int> a(1, 2, 3, 4);
		ruis::corners<int> b(1, 2, 3, 4);
		tst::check(!(a < b), SL);
		tst::check(!(b < a), SL);
	});

	suite.add("operator_less__differ_in_first_corner", [] {
		ruis::corners<int> a(1, 2, 3, 4);
		ruis::corners<int> b(2, 2, 3, 4);
		tst::check(a < b, SL);
		tst::check(!(b < a), SL);
	});

	suite.add("operator_less__differ_in_second_corner", [] {
		ruis::corners<int> a(1, 2, 3, 4);
		ruis::corners<int> b(1, 3, 3, 4);
		tst::check(a < b, SL);
		tst::check(!(b < a), SL);
	});

	suite.add("operator_less__differ_in_third_corner", [] {
		ruis::corners<int> a(1, 2, 3, 4);
		ruis::corners<int> b(1, 2, 4, 4);
		tst::check(a < b, SL);
		tst::check(!(b < a), SL);
	});

	suite.add("operator_less__differ_in_fourth_corner", [] {
		ruis::corners<int> a(1, 2, 3, 4);
		ruis::corners<int> b(1, 2, 3, 5);
		tst::check(a < b, SL);
		tst::check(!(b < a), SL);
	});

	suite.add("operator_less__lexicographic_order", [] {
		// first difference dominates
		ruis::corners<int> a(1, 9, 9, 9);
		ruis::corners<int> b(2, 0, 0, 0);
		tst::check(a < b, SL);
	});

	suite.add("corners_with_float", [] {
		ruis::corners<float> c(1.5f, 2.5f, 3.5f, 4.5f);
		tst::check_eq(c.left_top(), 1.5f, SL);
		tst::check_eq(c.right_top(), 2.5f, SL);
		tst::check_eq(c.right_bottom(), 3.5f, SL);
		tst::check_eq(c.left_bottom(), 4.5f, SL);
		tst::check_eq(c.top(), 4.0f, SL);
		tst::check_eq(c.bottom(), 8.0f, SL);
		tst::check_eq(c.left(), 6.0f, SL);
		tst::check_eq(c.right(), 6.0f, SL);
	});

});
}
