/*
morda GUI framework

Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

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

#include <vector>

#include <r4/vector.hpp>

#include <utki/span.hpp>

#include "texture_2d.hpp"

#include "vertex_buffer.hpp"
#include "vertex_array.hpp"
#include "index_buffer.hpp"

#include "texturing_shader.hpp"
#include "coloring_shader.hpp"
#include "shader.hpp"
#include "coloring_texturing_shader.hpp"
#include "frame_buffer.hpp"

namespace morda{

class render_factory{
protected:
	render_factory(){}
	
public:
	virtual ~render_factory()noexcept{}	
	
	virtual std::shared_ptr<texture_2d> create_texture_2d(texture_2d::type type, r4::vector2<unsigned> dims, utki::span<const uint8_t> data) = 0;
	
	std::shared_ptr<texture_2d> create_texture_2d(r4::vector2<unsigned> dims, utki::span<const uint32_t> data);
	
	virtual std::shared_ptr<vertex_buffer> create_vertex_buffer(utki::span<const r4::vector4<float>> vertices) = 0;
	
	virtual std::shared_ptr<vertex_buffer> create_vertex_buffer(utki::span<const r4::vector3<float>> vertices) = 0;
	
	virtual std::shared_ptr<vertex_buffer> create_vertex_buffer(utki::span<const r4::vector2<float>> vertices) = 0;
	
	virtual std::shared_ptr<vertex_buffer> create_vertex_buffer(utki::span<const float> vertices) = 0;
	
	virtual std::shared_ptr<index_buffer> create_index_buffer(utki::span<const uint16_t> indices) = 0;
	
	virtual std::shared_ptr<vertex_array> create_vertex_array(
			std::vector< std::shared_ptr<morda::vertex_buffer> >&& buffers,
			std::shared_ptr<morda::index_buffer> indices,
			vertex_array::mode rendering_mode
		) = 0;
	
	struct shaders{
		std::unique_ptr<texturing_shader> pos_tex;
		std::unique_ptr<coloring_shader> color_pos;
		std::unique_ptr<coloring_shader> color_pos_lum;
		std::unique_ptr<shader> pos_clr;
		std::unique_ptr<coloring_texturing_shader> color_pos_tex;
	};
	
	virtual std::unique_ptr<shaders> create_shaders() = 0;
	
	virtual std::shared_ptr<frame_buffer> create_framebuffer(std::shared_ptr<texture_2d> color) = 0;
};

}
