/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include <r4/rectangle.hpp>
#include <r4/vector.hpp>
#include <ruis/util/to_tml.hpp>
#include <tst/check.hpp>
#include <tst/set.hpp>

namespace {
const tst::set set("to_tml", [](tst::suite& suite) {
	suite.add("vector2_produces_forest_with_x_and_y_component_trees", [] {
		r4::vector2<int> v(1, 2);
		const auto f = ruis::to_tml(v);
		tst::check_eq(f.size(), std::size_t(2), SL);
		tst::check(
			f[0].value == "x",
			[&](auto& o) {
				o << f[0].value;
			},
			SL
		);
		tst::check_eq(f[0].children.size(), std::size_t(1), SL);
		tst::check(
			f[0].children[0].value == "1",
			[&](auto& o) {
				o << f[0].children[0].value;
			},
			SL
		);
		tst::check(
			f[1].value == "y",
			[&](auto& o) {
				o << f[1].value;
			},
			SL
		);
		tst::check_eq(f[1].children.size(), std::size_t(1), SL);
		tst::check(
			f[1].children[0].value == "2",
			[&](auto& o) {
				o << f[1].children[0].value;
			},
			SL
		);
	});

	suite.add("vector3_produces_forest_with_x_y_and_z_component_trees", [] {
		r4::vector3<int> v(1, 2, 3);
		const auto f = ruis::to_tml(v);
		tst::check_eq(f.size(), std::size_t(3), SL);
		tst::check(
			f[0].value == "x",
			[&](auto& o) {
				o << f[0].value;
			},
			SL
		);
		tst::check(
			f[1].value == "y",
			[&](auto& o) {
				o << f[1].value;
			},
			SL
		);
		tst::check(
			f[2].value == "z",
			[&](auto& o) {
				o << f[2].value;
			},
			SL
		);
		tst::check(
			f[0].children[0].value == "1",
			[&](auto& o) {
				o << f[0].children[0].value;
			},
			SL
		);
		tst::check(
			f[1].children[0].value == "2",
			[&](auto& o) {
				o << f[1].children[0].value;
			},
			SL
		);
		tst::check(
			f[2].children[0].value == "3",
			[&](auto& o) {
				o << f[2].children[0].value;
			},
			SL
		);
	});

	suite.add("vector4_produces_forest_with_x_y_z_and_w_component_trees", [] {
		r4::vector4<int> v(1, 2, 3, 4);
		const auto f = ruis::to_tml(v);
		tst::check_eq(f.size(), std::size_t(4), SL);
		tst::check(
			f[0].value == "x",
			[&](auto& o) {
				o << f[0].value;
			},
			SL
		);
		tst::check(
			f[1].value == "y",
			[&](auto& o) {
				o << f[1].value;
			},
			SL
		);
		tst::check(
			f[2].value == "z",
			[&](auto& o) {
				o << f[2].value;
			},
			SL
		);
		tst::check(
			f[3].value == "w",
			[&](auto& o) {
				o << f[3].value;
			},
			SL
		);
		tst::check(
			f[0].children[0].value == "1",
			[&](auto& o) {
				o << f[0].children[0].value;
			},
			SL
		);
		tst::check(
			f[1].children[0].value == "2",
			[&](auto& o) {
				o << f[1].children[0].value;
			},
			SL
		);
		tst::check(
			f[2].children[0].value == "3",
			[&](auto& o) {
				o << f[2].children[0].value;
			},
			SL
		);
		tst::check(
			f[3].children[0].value == "4",
			[&](auto& o) {
				o << f[3].children[0].value;
			},
			SL
		);
	});

	suite.add("number_produces_tree_with_decimal_value", [] {
		const auto tx = ruis::to_tml(42);
		tst::check_eq(tx.children.size(), std::size_t(0), SL);
		tst::check(
			tx.value == "42",
			[&](auto& o) {
				o << tx.value;
			},
			SL
		);
		const auto ty = ruis::to_tml(1.5f);
		tst::check(
			ty.value == "1.5",
			[&](auto& o) {
				o << ty.value;
			},
			SL
		);
	});

	suite.add("bool_produces_tree_with_true_or_false", [] {
		tst::check(
			ruis::to_tml(true).value == "true",
			[&](auto& o) {
				o << ruis::to_tml(true).value;
			},
			SL
		);
		tst::check(
			ruis::to_tml(false).value == "false",
			[&](auto& o) {
				o << ruis::to_tml(false).value;
			},
			SL
		);
	});

	suite.add("rectangle_produces_rect_tree_with_p_and_d", [] {
		r4::rectangle<int> r(1, 2, 3, 4); // pos (1,2), dims (3,4)
		const auto t = ruis::to_tml(r);
		tst::check(
			t.value == "rect",
			[&](auto& o) {
				o << t.value;
			},
			SL
		);
		tst::check_eq(t.children.size(), std::size_t(2), SL);
		tst::check(
			t.children[0].value == "p",
			[&](auto& o) {
				o << t.children[0].value;
			},
			SL
		);
		tst::check(
			t.children[1].value == "d",
			[&](auto& o) {
				o << t.children[1].value;
			},
			SL
		);
		// each of p/d is a vector with x/y component nodes
		tst::check_eq(t.children[0].children.size(), std::size_t(2), SL);
		tst::check(t.children[0].children[0].value == "x", SL);
		tst::check(
			t.children[0].children[0].children[0].value == "1",
			[&](auto& o) {
				o << t.children[0].children[0].children[0].value;
			},
			SL
		);
		tst::check(t.children[0].children[1].value == "y", SL);
		tst::check(
			t.children[0].children[1].children[0].value == "2",
			[&](auto& o) {
				o << t.children[0].children[1].children[0].value;
			},
			SL
		);
		tst::check_eq(t.children[1].children.size(), std::size_t(2), SL);
		tst::check(t.children[1].children[0].value == "x", SL);
		tst::check(
			t.children[1].children[0].children[0].value == "3",
			[&](auto& o) {
				o << t.children[1].children[0].children[0].value;
			},
			SL
		);
		tst::check(t.children[1].children[1].value == "y", SL);
		tst::check(
			t.children[1].children[1].children[0].value == "4",
			[&](auto& o) {
				o << t.children[1].children[1].children[0].value;
			},
			SL
		);
	});
});
} // namespace
