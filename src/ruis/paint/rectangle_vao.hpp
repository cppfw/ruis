/*
ruis - GUI framework

Copyright (C) 2012-2025  Ivan Gagis <igagis@gmail.com>

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

#include "../config.hpp"
#include "../util/sides.hpp"
#include "../render/renderer.hpp"

namespace ruis::paint{

// TODO: doxygen
class rectangle_vao{
    utki::shared_ref<const ruis::render::renderer> renderer;

    ruis::sides<ruis::real> corner_radii = 0;

    // zero means filled rectangle
    ruis::real stroke_width = 0; // TODO: use
public:
    rectangle_vao(
        utki::shared_ref<const ruis::render::renderer> renderer,
        ruis::sides<ruis::real> corner_radii = 0, //
        ruis::real stroke_width = 0
    );

    rectangle_vao(const rectangle_vao&) = delete;
    rectangle_vao& operator=(const rectangle_vao&) = delete;

    rectangle_vao(rectangle_vao&&) = delete;
    rectangle_vao& operator=(rectangle_vao&&) = delete;

    ~rectangle_vao() = default;

    void set(
        ruis::sides<ruis::real> corner_radii, //
        ruis::real stroke_width
    );

    void render(
        const ruis::matrix4& matrix,//
        const vec2& dims,
		const ruis::color& color
    ) const;

private:
    void render_rounder_corners(
        const mat4& matrix,
        const vec2& dims,
		const ruis::color& color
    ) const;

	struct rounded_corners_texture {
		utki::shared_ref<const render::texture_2d> tex;

		// texture middle point
		vec2 middle_px;

		// vaos for corners
		std::array<std::array<utki::shared_ref<const render::vertex_array>, 2>, 2> vaos;

		rounded_corners_texture(
			const ruis::render::renderer& r, //
			utki::shared_ref<const render::texture_2d> tex,
			vec2 middle_px
		);

	private:
		rounded_corners_texture(
			const ruis::render::renderer& r, //
			vec2 middle,
			utki::shared_ref<const render::texture_2d>& tex,
			vec2 middle_px
		);
	};

	static std::map<
		sides<real>, //
		std::weak_ptr<rounded_corners_texture>>
		// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables, "false-positive")
		cache;

	std::shared_ptr<rounded_corners_texture> rounded_corners_tex;

	void update_rounded_corners_texture();
};

}
