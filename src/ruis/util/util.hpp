/*
ruis - GUI framework

Copyright (C) 2012-2024  Ivan Gagis <igagis@gmail.com>

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

#include <r4/rectangle.hpp>
#include <r4/vector.hpp>
#include <tml/tree.hpp>
#include <utki/config.hpp>

#include "../config.hpp"
#include "../layouts/layout_parameters.hpp"

#include "units.hpp"

namespace ruis {

/**
 * @brief Make viewport matrix.
 * Viewport matrix is a matrix which sets up a right-handed coordinate system with:
 * - x-axis right, y-axis down, z-axis away
 * - zero at top-left corner of the viewport
 * - viewport right and bottom edges correspond to coordinates equal to viewport size in pixels.
 *
 * @param initial - renderer's initial matrix.
 * @param viewport_dim - dimensions of the viewport.
 * @return Viewport matrix.
 */
inline ruis::matrix4 make_viewport_matrix(const ruis::matrix4& initial, ruis::vector2 viewport_dim)
{
	ruis::matrix4 m(initial);

	// viewport right and bottom edges correspond to coordinates equal to viewport size in pixels.
	m.scale(vector2(1).comp_divide(viewport_dim));

	return m;
}

ruis::vector2 parse_vec2(tml::forest::const_iterator begin, tml::forest::const_iterator end);

/**
 * @brief Parse 2 values from tml as vector2.
 * @param desc - forest of at least two trees holding vector2 values.
 *                If there are less than 2 trees in the forest then the rest of
 *                vector components will be filled with latest parsed value.
 * @return parsed vector2.
 */
inline ruis::vector2 parse_vec2(const tml::forest& desc)
{
	return parse_vec2(desc.begin(), desc.end());
}

/**
 * @brief Parse chain of 4 tml nodes as rectangle.
 * @param desc - chain of at least four nodes holding rectangle values.
 *               If there are less than 4 nodes in the chain then the rest of
 *               rectangle components will be filled with latest parsed value.
 *               If zero pointer is passed the resulting rectangle will be filled with zeros.
 * @return parsed rectangle.
 */
ruis::rect parse_rect(const tml::forest& desc);

/**
 * @brief Parse chain of 4 tml nodes as sides.
 * @param desc - chain of at least four nodes holding sides values.
 *               If there are less than 4 nodes in the chain then the rest of
 *               sides<real> components will be filled with latest parsed value.
 *               If zero pointer is passed the resulting sides<real> will be filled with zeros.
 * @return parsed sides<real>.
 */
ruis::sides<real> parse_sides(const tml::forest& desc);

/**
 * @brief Parse dimension value.
 * Parses value of dimension property from tml leaf.
 * In case the value is given in millimeters or points it will do the conversion.
 * @param l - tml leaf holding the value.
 * @param units - information about units. Can be obtained from context.
 * @return Parsed value in pixels.
 */
length parse_dimension_value(const tml::leaf& l, const ruis::units& units);

/**
 * @brief Parse layout dimension value.
 * Parses value of dimension value of layout parameters from tml.
 * @param l - tml leaf holding the value.
 * @param units - information about units. Can be obtained from context.
 * @return Parsed value.
 */
ruis::lp::dimension parse_layout_dimension_value(const tml::leaf& l, const ruis::units& units);

bool is_property(const tml::tree& t);

bool is_leaf_property(const tml::leaf& l);

bool is_leaf_child(const tml::leaf& l);

bool is_variable(const tml::tree& t);

r4::vector4<float> color_to_vec4f(uint32_t color);

inline const tml::leaf& get_property_value(const tml::tree& p)
{
	if (p.children.size() != 1) {
		throw std::invalid_argument("get_property_value(): property has no value");
	}
	return p.children.front().value;
}

} // namespace ruis
