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

#include <r4/vector.hpp>
#include <tml/tree.hpp>

#include "../config.hpp"
#include "../render/vertex_array.hpp"
#include "../resource_loader.hpp"
#include "../util/color.hpp"

namespace ruis::res {

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
 * ruis_grd_window_background{
 *     stop{ pos{0} color{0} }
 *     stop{ pos{0.43} color{0x10ffffff} }
 *     stop{ pos{0.5} color{0x12ffffff} }
 *     stop{ pos{0.57} color{0x10ffffff} }
 *     stop{ pos{1} color{0} }
 * }
 * @endcode
 */
class gradient : public resource
{
	friend class ruis::resource_loader;

private:
	utki::shared_ref<const render::vertex_array> vao;

	static utki::shared_ref<const render::vertex_array> make_vao(
		const ruis::render::context& rendering_context, //
		utki::span<const std::tuple<real, color>> stops,
		bool vertical
	);

public:
	/**
	 * @brief Create gradient resource form aray of gradient stops.
	 * A gradient stop is a pair of values. First one is a floating point value
	 * from [0:1] defining the position of the gradient stop. The second value
	 * defines the color of the stop.
	 * @param rendering_context - ruis rendering context.
	 */
	gradient(
		const ruis::render::context& rendering_context, //
		utki::span<const std::tuple<real, color>> stops,
		bool vertical
	);

	gradient(const gradient&) = delete;
	gradient& operator=(const gradient&) = delete;

	gradient(gradient&&) = delete;
	gradient& operator=(gradient&&) = delete;

	~gradient() override = default;

	/**
	 * @brief render gradient.
	 * Renders the gradient as a rectangle ((0,0),(1,1)).
	 * @param m - transformation matrix.
	 */
	void render(
		ruis::render::renderer& renderer, //
		const ruis::mat4& m
	) const;

private:
	static utki::shared_ref<gradient> load(
		const ruis::resource_loader& loader, //
		const ::tml::forest& desc,
		const fsif::file& fi
	);
};

} // namespace ruis::res
