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

#include "context.hpp"

namespace ruis::render {

class renderer : public std::enable_shared_from_this<renderer>
{
public:
	const utki::shared_ref<ruis::render::context> render_context;

	struct objects {
		utki::shared_ref<const ruis::render::context::shaders> shaders;
		utki::shared_ref<const vertex_array> empty_vertex_array;
		utki::shared_ref<const vertex_buffer> quad_01_vbo;
		utki::shared_ref<const index_buffer> quad_indices;
		utki::shared_ref<const vertex_array> pos_quad_01_vao;
		utki::shared_ref<const vertex_array> pos_tex_quad_01_vao;
		utki::shared_ref<const texture_2d> white_texture;
	};

	const utki::shared_ref<const objects> common_objects;

	const objects& obj() const noexcept
	{
		return this->common_objects.get();
	}

	const ruis::render::context::shaders& shaders() const noexcept
	{
		return this->common_objects.get().shaders.get();
	}

	renderer(utki::shared_ref<ruis::render::context> render_context);

	renderer(
		utki::shared_ref<ruis::render::context> render_context, //
		utki::shared_ref<objects> common_objects
	);

	renderer(const renderer&) = delete;
	renderer& operator=(const renderer&) = delete;

	renderer(renderer&&) = delete;
	renderer& operator=(renderer&&) = delete;

	virtual ~renderer() = default;
};

} // namespace ruis::render
