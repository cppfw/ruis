/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#include <r4/vector.hpp>

#include <treeml/tree.hpp>

#include "../resource_loader.hpp"
#include "../config.hpp"

#include "../render/vertex_array.hpp"

#include "../util/util.hpp"

namespace morda{ namespace res{

/**
 * @brief Gradient resource.
 * 
 * %resource description:
 * 
 * %List of stops.
 * 
 * Stop parameters:
 * 
 * @param pos - stop position from range [0:1].
 * @param color - stop color.
 * 
 * Example:
 * @code
 * morda_grd_window_background{
 *     Stop{ pos{0} color{0} }
 *     Stop{ pos{0.43} color{0x10ffffff} }
 *     Stop{ pos{0.5} color{0x12ffffff} }
 *     Stop{ pos{0.57} color{0x10ffffff} }
 *     Stop{ pos{1} color{0} }
 * }
 * @endcode
 */
class gradient : public resource{
	friend class morda::resource_loader;

	utki::shared_ref<const vertex_array> vao;
	
public:
	/**
	 * @brief Create gradient resource form aray of gradient stops.
	 * A gradient stop is a pair of values. First one is a floating point value
	 * from [0:1] defining the position of the gradient stop. The second value
	 * defines the color of the stop.
	 * @param c - context.
	 */
	gradient(
		const utki::shared_ref<morda::context>& c
	);
	
	gradient(const gradient&) = delete;
	gradient& operator=(const gradient&) = delete;
	
	/**
	 * @brief Set gradient.
	 * @param stops - array of gradient stops.
	 * @param vertical - if true, the gradient is vertical. If false, the gradient is horizontal.
	 */
	void set(
		std::vector<std::tuple<real, uint32_t>>& stops,
		bool vertical
	);

	/**
	 * @brief render gradient.
	 * Renders the gradient as a rectangle ((0,0),(1,1)).
	 * @param m - transformation matrix.
	 */
	void render(const morda::matrix4& m)const;
	
private:
	static utki::shared_ref<gradient> load(const utki::shared_ref<morda::context>& ctx, const ::treeml::forest& desc, const papki::file& fi);
};

}}
