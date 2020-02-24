#pragma once

#include <vector>

#include <r4/vector2.hpp>

#include <utki/shared.hpp>
#include <utki/Unique.hpp>
#include <utki/Buf.hpp>

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
	
	virtual std::shared_ptr<texture_2d> create_texture_2d(texture_2d::type type, r4::vec2ui dims, const utki::span<uint8_t> data) = 0;
	
	std::shared_ptr<texture_2d> create_texture_2d(r4::vec2ui dims, const utki::span<uint32_t> data);
	
	virtual std::shared_ptr<vertex_buffer> create_vertex_buffer(const utki::span<r4::vec4f> vertices) = 0;
	
	virtual std::shared_ptr<vertex_buffer> create_vertex_buffer(const utki::span<r4::vec3f> vertices) = 0;
	
	virtual std::shared_ptr<vertex_buffer> create_vertex_buffer(const utki::span<r4::vec2f> vertices) = 0;
	
	virtual std::shared_ptr<vertex_buffer> create_vertex_buffer(const utki::span<float> vertices) = 0;
	
	virtual std::shared_ptr<index_buffer> create_index_buffer(const utki::span<uint16_t> indices) = 0;
	
	virtual std::shared_ptr<vertex_array> create_vertex_array(
			std::vector< std::shared_ptr<morda::vertex_buffer> >&& buffers,
			std::shared_ptr<morda::index_buffer> indices,
			vertex_array::Mode_e mode
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
