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

#include "../config.hpp"
#include "../render/renderer.hpp"

namespace ruis::paint {
class oval_vao
{
	utki::shared_ref<const ruis::render::renderer> renderer;

	vec2 radii = 0;

	// zero means filled rectangle
	real stroke_width = 0; // TODO: use

public:
	oval_vao(
		utki::shared_ref<const ruis::render::renderer> renderer,
		vec2 radii = 0, //
		real stroke_width = 0
	);

	oval_vao(const oval_vao&) = delete;
	oval_vao& operator=(const oval_vao&) = delete;

	oval_vao(oval_vao&&) = delete;
	oval_vao& operator=(oval_vao&&) = delete;

	~oval_vao() = default;

	void set(
		vec2 radii, //
		real stroke_width = 0 // 0 means filled ellipse
	);

	void render(
		const ruis::mat4& matrix, //
		const ruis::color& color
	) const;

private:
	std::shared_ptr<const render::texture_2d> tex;

	static std::map<
		vec2, // radii
		std::weak_ptr<const render::texture_2d>>
		// TODO: ? NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables, "false-positive")
		cache;

	void update_texture();
};
} // namespace ruis::paint
