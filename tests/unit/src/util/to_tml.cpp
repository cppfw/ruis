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

#include <r4/vector.hpp>
#include <tst/check.hpp>
#include <tst/set.hpp>

#include <ruis/util/to_tml.hpp>

namespace {
const tst::set set("to_tml", [](tst::suite& suite) {
	suite.add("vector2_produces_forest_with_x_and_y_component_trees", [] {
		r4::vector2<int> v(1, 2);
		const auto f = ruis::to_tml(v);
		tst::check_eq(f.size(), std::size_t(2), SL);
		tst::check(f[0].value == "x", SL) << f[0].value;
		tst::check_eq(f[0].children.size(), std::size_t(1), SL);
		tst::check(f[0].children[0].value == "1", SL) << f[0].children[0].value;
		tst::check(f[1].value == "y", SL) << f[1].value;
		tst::check_eq(f[1].children.size(), std::size_t(1), SL);
		tst::check(f[1].children[0].value == "2", SL) << f[1].children[0].value;
	});

	suite.add("vector3_produces_forest_with_x_y_and_z_component_trees", [] {
		r4::vector3<int> v(1, 2, 3);
		const auto f = ruis::to_tml(v);
		tst::check_eq(f.size(), std::size_t(3), SL);
		tst::check(f[0].value == "x", SL) << f[0].value;
		tst::check(f[1].value == "y", SL) << f[1].value;
		tst::check(f[2].value == "z", SL) << f[2].value;
		tst::check(f[0].children[0].value == "1", SL) << f[0].children[0].value;
		tst::check(f[1].children[0].value == "2", SL) << f[1].children[0].value;
		tst::check(f[2].children[0].value == "3", SL) << f[2].children[0].value;
	});

	suite.add("vector4_produces_forest_with_x_y_z_and_w_component_trees", [] {
		r4::vector4<int> v(1, 2, 3, 4);
		const auto f = ruis::to_tml(v);
		tst::check_eq(f.size(), std::size_t(4), SL);
		tst::check(f[0].value == "x", SL) << f[0].value;
		tst::check(f[1].value == "y", SL) << f[1].value;
		tst::check(f[2].value == "z", SL) << f[2].value;
		tst::check(f[3].value == "w", SL) << f[3].value;
		tst::check(f[0].children[0].value == "1", SL) << f[0].children[0].value;
		tst::check(f[1].children[0].value == "2", SL) << f[1].children[0].value;
		tst::check(f[2].children[0].value == "3", SL) << f[2].children[0].value;
		tst::check(f[3].children[0].value == "4", SL) << f[3].children[0].value;
	});

	suite.add("number_produces_tree_with_decimal_value", [] {
		const auto tx = ruis::to_tml(42);
		tst::check_eq(tx.children.size(), std::size_t(0), SL);
		tst::check(tx.value == "42", SL) << tx.value;
		const auto ty = ruis::to_tml(1.5f);
		tst::check(ty.value == "1.5", SL) << ty.value;
	});
});
} // namespace
