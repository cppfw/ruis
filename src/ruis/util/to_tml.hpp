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

#pragma once

#include <cstddef>
#include <type_traits>

#include <r4/vector.hpp>
#include <tml/tree.hpp>

namespace ruis {

/**
 * @brief Convert a number to a tml::tree.
 * The tree value is the number formatted by the corresponding
 * tml::leaf constructor.
 * Built-in arithmetic types have no associated namespaces,
 * so ADL cannot find a to_tml() for them - this free template
 * is the only provider of to_tml() for numbers and is what
 * lets them participate in the generic to_tml() scheme
 * (it is also found by ordinary name lookup from the other
 * to_tml() overloads in this header).
 * @tparam number_type - an arithmetic type.
 * @param n - the number to convert.
 * @return A tml::tree representing the number.
 */
template <class number_type, std::enable_if_t<std::is_arithmetic_v<number_type>, int> = 0>
tml::tree to_tml(const number_type& n)
{
	return tml::tree(tml::leaf(n));
}

/**
 * @brief Convert an r4::vector to a tml::forest.
 * The forest contains one node per vector component,
 * each node is named after the component ("x", "y", "z", "w")
 * and contains the tree produced by the component's to_tml()
 * (found by ordinary name lookup for arithmetic component types
 * and by ADL for class component types).
 * @tparam component_type - the type of the vector components,
 * must be an arithmetic type or have a free to_tml() overload
 * findable by ADL.
 * @tparam dimension - the number of vector components.
 * @param v - the vector to convert.
 * @return A tml::forest containing the trees of the vector components.
 */
template <class component_type, std::size_t dimension>
tml::forest to_tml(const r4::vector<component_type, dimension>& v)
{
	static_assert(dimension <= 4, "vectors with more than 4 components are not supported for now");
	static constexpr const char* component_names[] = {"x", "y", "z", "w"};
	tml::forest children;
	for (std::size_t i = 0; i < dimension; ++i) {
		// found by ordinary name lookup or ADL (see the template documentation)
		children.emplace_back(component_names[i], tml::forest{to_tml(v[i])});
	}
	return children;
}

} // namespace ruis
