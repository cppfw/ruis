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

#include "../render/renderer.hpp"
#include "../util/color.hpp"

#include "path.hpp"

namespace ruis::paint {

// TODO: doxygen
class path_vao
{
	utki::shared_ref<const ruis::render::renderer> renderer;

	utki::shared_ref<const ruis::render::vertex_array> core;
	utki::shared_ref<const ruis::render::vertex_array> border;

public:
	path_vao(utki::shared_ref<const ruis::render::renderer> r);

	path_vao(const path_vao&) = delete;
	path_vao& operator=(const path_vao&) = delete;

	path_vao(path_vao&&) = default;
	path_vao& operator=(path_vao&&) = default;

	~path_vao() = default;

	void set(const path::vertices& path);

	void render(
		const ruis::mat4& matrix, //
		ruis::color color
	) const;
};

} // namespace ruis::paint
