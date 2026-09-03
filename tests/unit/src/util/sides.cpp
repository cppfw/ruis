#include <tst/set.hpp>
#include <tst/check.hpp>

#include <ruis/util/sides.hpp>

namespace {
const tst::set set("sides", [](tst::suite& suite) {
	suite.add("single_value_constructor_sets_all_sides", [] {
		ruis::sides<int> s(5);
		tst::check_eq(s.left(), 5, SL);
		tst::check_eq(s.top(), 5, SL);
		tst::check_eq(s.right(), 5, SL);
		tst::check_eq(s.bottom(), 5, SL);
	});

	suite.add("four_value_constructor_sets_each_side", [] {
		ruis::sides<int> s(1, 2, 3, 4);
		tst::check_eq(s.left(), 1, SL);
		tst::check_eq(s.top(), 2, SL);
		tst::check_eq(s.right(), 3, SL);
		tst::check_eq(s.bottom(), 4, SL);
	});

	suite.add("index_access_matches_named_access", [] {
		ruis::sides<int> s(10, 20, 30, 40);
		tst::check_eq(s[0], s.left(), SL);
		tst::check_eq(s[1], s.top(), SL);
		tst::check_eq(s[2], s.right(), SL);
		tst::check_eq(s[3], s.bottom(), SL);
	});

	suite.add("mutable_named_accessors_allow_modification", [] {
		ruis::sides<int> s(0);
		s.left() = 1;
		s.top() = 2;
		s.right() = 3;
		s.bottom() = 4;
		tst::check_eq(s.left(), 1, SL);
		tst::check_eq(s.top(), 2, SL);
		tst::check_eq(s.right(), 3, SL);
		tst::check_eq(s.bottom(), 4, SL);
	});

	suite.add("dim_x_is_sum_of_left_and_right", [] {
		ruis::sides<int> s(1, 2, 3, 4);
		tst::check_eq(s.dim_x(), 4, SL);
	});

	suite.add("dim_y_is_sum_of_top_and_bottom", [] {
		ruis::sides<int> s(1, 2, 3, 4);
		tst::check_eq(s.dim_y(), 6, SL);
	});

	suite.add("left_top_returns_correct_vector", [] {
		ruis::sides<int> s(1, 2, 3, 4);
		auto v = s.left_top();
		tst::check_eq(v.x(), 1, SL);
		tst::check_eq(v.y(), 2, SL);
	});

	suite.add("right_top_returns_correct_vector", [] {
		ruis::sides<int> s(1, 2, 3, 4);
		auto v = s.right_top();
		tst::check_eq(v.x(), 3, SL);
		tst::check_eq(v.y(), 2, SL);
	});

	suite.add("right_bottom_returns_correct_vector", [] {
		ruis::sides<int> s(1, 2, 3, 4);
		auto v = s.right_bottom();
		tst::check_eq(v.x(), 3, SL);
		tst::check_eq(v.y(), 4, SL);
	});

	suite.add("left_bottom_returns_correct_vector", [] {
		ruis::sides<int> s(1, 2, 3, 4);
		auto v = s.left_bottom();
		tst::check_eq(v.x(), 1, SL);
		tst::check_eq(v.y(), 4, SL);
	});

	suite.add("dims_returns_correct_vector", [] {
		ruis::sides<int> s(1, 2, 3, 4);
		auto v = s.dims();
		tst::check_eq(v.x(), 4, SL);
		tst::check_eq(v.y(), 6, SL);
	});

	suite.add("is_zero_returns_true_for_all_zeros", [] {
		ruis::sides<int> s(0);
		tst::check(s.is_zero(), SL);
	});

	suite.add("is_zero_returns_false_when_any_side_nonzero", [] {
		ruis::sides<int> s(0, 0, 0, 1);
		tst::check(!s.is_zero(), SL);
	});

	suite.add("operator_assign_sets_all_sides", [] {
		ruis::sides<int> s(1, 2, 3, 4);
		s = 7;
		tst::check_eq(s.left(), 7, SL);
		tst::check_eq(s.top(), 7, SL);
		tst::check_eq(s.right(), 7, SL);
		tst::check_eq(s.bottom(), 7, SL);
	});

	suite.add("equality_via_array_inheritance", [] {
		ruis::sides<int> a(1, 2, 3, 4);
		ruis::sides<int> b(1, 2, 3, 4);
		ruis::sides<int> c(1, 2, 3, 5);
		tst::check(a == b, SL);
		tst::check(a != c, SL);
	});

	suite.add("operator_less__equal", [] {
		ruis::sides<int> a(1, 2, 3, 4);
		ruis::sides<int> b(1, 2, 3, 4);
		tst::check(!(a < b), SL);
		tst::check(!(b < a), SL);
	});

	suite.add("operator_less__differ_in_first_side", [] {
		ruis::sides<int> a(1, 2, 3, 4);
		ruis::sides<int> b(2, 2, 3, 4);
		tst::check(a < b, SL);
		tst::check(!(b < a), SL);
	});

	suite.add("operator_less__differ_in_second_side", [] {
		ruis::sides<int> a(1, 2, 3, 4);
		ruis::sides<int> b(1, 3, 3, 4);
		tst::check(a < b, SL);
		tst::check(!(b < a), SL);
	});

	suite.add("operator_less__differ_in_third_side", [] {
		ruis::sides<int> a(1, 2, 3, 4);
		ruis::sides<int> b(1, 2, 4, 4);
		tst::check(a < b, SL);
		tst::check(!(b < a), SL);
	});

	suite.add("operator_less__differ_in_fourth_side", [] {
		ruis::sides<int> a(1, 2, 3, 4);
		ruis::sides<int> b(1, 2, 3, 5);
		tst::check(a < b, SL);
		tst::check(!(b < a), SL);
	});

	suite.add("operator_less__lexicographic_order", [] {
		// first difference dominates
		ruis::sides<int> a(1, 9, 9, 9);
		ruis::sides<int> b(2, 0, 0, 0);
		tst::check(a < b, SL);
	});

	suite.add("sides_with_float", [] {
		ruis::sides<float> s(1.5f, 2.5f, 3.5f, 4.5f);
		tst::check_eq(s.left(), 1.5f, SL);
		tst::check_eq(s.top(), 2.5f, SL);
		tst::check_eq(s.right(), 3.5f, SL);
		tst::check_eq(s.bottom(), 4.5f, SL);
		tst::check_eq(s.dim_x(), 5.0f, SL);
		tst::check_eq(s.dim_y(), 7.0f, SL);
	});

});
}
